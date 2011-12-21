/*
 * Copyright (c) 1997-2003 Erez Zadok
 * Copyright (c) 2001-2003 Stony Brook University
 *
 * For specific licensing information, see the COPYING file distributed with
 * this package, or get one from ftp://ftp.filesystems.org/pub/fist/COPYING.
 *
 * This Copyright notice must be kept intact and distributed with all
 * fistgen sources INCLUDING sources generated by fistgen.
 */
/*
 * Copyright (C) 2004, 2005 Markus Klotzbuecher <mk@creamnet.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

/*
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "fist.h"
#include "mini_fo.h"


/*******************
 * File Operations *
 *******************/

STATIC loff_t
mini_fo_llseek(file_t *file, loff_t offset, int origin)
{
	loff_t err;
	file_t *hidden_file = NULL;

	if(S_ISDIR(file->f_dentry->d_inode->i_mode)) {
		/* Check if trying to llseek from a directory */
		err = -EISDIR;
		goto out;
	}
	if (ftopd(file) != NULL) {
		if(ftohf2(file)) {
			hidden_file = ftohf2(file);
		} else {
			hidden_file = ftohf(file);
		}
	}

	/* always set hidden position to this one */
	hidden_file->f_pos = file->f_pos;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	memcpy(&(hidden_file->f_ra),
	       &(file->f_ra),
	       sizeof(struct file_ra_state));
#else
	if (file->f_reada) { /* update readahead information if needed */
		hidden_file->f_reada = file->f_reada;
		hidden_file->f_ramax = file->f_ramax;
		hidden_file->f_raend = file->f_raend;
		hidden_file->f_ralen = file->f_ralen;
		hidden_file->f_rawin = file->f_rawin;
	}
#endif
	if (hidden_file->f_op && hidden_file->f_op->llseek)
		err = hidden_file->f_op->llseek(hidden_file, offset, origin);
	else
		err = generic_file_llseek(hidden_file, offset, origin);

	if (err < 0)
		goto out;

	if (err != file->f_pos) {
		file->f_pos = err;
		// ION maybe this?
		//	file->f_pos = hidden_file->f_pos;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		file->f_reada = 0;
#endif
		file->f_version++;
	}

 out:
	return err;
}


/* mk: fanout capable */
STATIC ssize_t
mini_fo_read(file_t *file, char *buf, size_t count, loff_t *ppos)
{
	int err = -EINVAL;
	file_t *hidden_file = NULL;
	loff_t pos = *ppos;

	if(S_ISDIR(file->f_dentry->d_inode->i_mode)) {
		/* Check if trying to read from a directory */
		/* printk(KERN_CRIT "mini_fo_read: ERROR: trying to read data from a directory.\n"); */
		err = -EISDIR;
		goto out;
	}

	if (ftopd(file) != NULL) {
		if(ftohf2(file)) {
			hidden_file = ftohf2(file);
		} else {
			hidden_file = ftohf(file);
		}
	}

	if (!hidden_file->f_op || !hidden_file->f_op->read)
		goto out;

	err = hidden_file->f_op->read(hidden_file, buf, count, &pos);
	*ppos = pos;

	if (err >= 0) {
		/* atime should also be updated for reads of size zero or more */
		fist_copy_attr_atime(file->f_dentry->d_inode,
				     hidden_file->f_dentry->d_inode);
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	/*
	 * MAJOR HACK
	 * because pread() does not have any way to tell us that it is
	 * our caller, then we don't know for sure if we have to update
	 * the file positions.  This hack relies on read() having passed us
	 * the "real" pointer of its struct file's f_pos field.
	 */
	if (ppos == &file->f_pos)
		hidden_file->f_pos = *ppos = pos;
	if (hidden_file->f_reada) { /* update readahead information if needed */
		file->f_reada = hidden_file->f_reada;
		file->f_ramax = hidden_file->f_ramax;
		file->f_raend = hidden_file->f_raend;
		file->f_ralen = hidden_file->f_ralen;
		file->f_rawin = hidden_file->f_rawin;
	}
#else
	memcpy(&(file->f_ra),&(hidden_file->f_ra),sizeof(struct file_ra_state));
#endif

 out:
	return err;
}


/* this mini_fo_write() does not modify data pages! */
STATIC ssize_t
mini_fo_write(file_t *file, const char *buf, size_t count, loff_t *ppos)
{
	int err = -EINVAL;
	file_t *hidden_file = NULL;
	inode_t *inode;
	inode_t *hidden_inode;
	loff_t pos = *ppos;

	/* mk: fan out: */
	if (ftopd(file) != NULL) {
		if(ftohf2(file)) {
			hidden_file = ftohf2(file);
		} else {
			/* This is bad! We have no storage file to write to. This
			 * should never happen because if a file is opened for
			 * writing, a copy should have been made earlier.
			 */
			printk(KERN_CRIT "mini_fo: write : ERROR, no storage file to write.\n");
			err = -EINVAL;
			goto out;
		}
	}

	inode = file->f_dentry->d_inode;
	hidden_inode = itohi2(inode);
	if(!hidden_inode) {
		printk(KERN_CRIT "mini_fo: write: no sto inode found, not good.\n");
		goto out;
	}

	if (!hidden_file->f_op || !hidden_file->f_op->write)
		goto out;

	/* adjust for append -- seek to the end of the file */
	if (file->f_flags & O_APPEND)
		pos = inode->i_size;

	err = hidden_file->f_op->write(hidden_file, buf, count, &pos);

	/*
	 * copy ctime and mtime from lower layer attributes
	 * atime is unchanged for both layers
	 */
	if (err >= 0)
		fist_copy_attr_times(inode, hidden_inode);

	*ppos = pos;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	/*
	 * XXX: MAJOR HACK
	 *
	 * because pwrite() does not have any way to tell us that it is
	 * our caller, then we don't know for sure if we have to update
	 * the file positions.  This hack relies on write() having passed us
	 * the "real" pointer of its struct file's f_pos field.
	 */
	if (ppos == &file->f_pos)
		hidden_file->f_pos = *ppos = pos;
#endif
	/* update this inode's size */
	if (pos > inode->i_size)
		inode->i_size = pos;

 out:
	return err;
}

/* Global variable to hold a file_t pointer.
 * This serves to allow mini_fo_filldir64 function to know which file is
 * beeing read, which is required for two reasons:
 *
 *   - be able to call wol functions in order to avoid listing deleted
 *     base files.
 *   - if we're reading a directory which is in state 1, we need to
 *     maintain a list (in mini_fo_filldir64) of which files allready
 *     have been copied to userspace,to detect files existing in base
 *     and storage and not list them twice.
 */
file_t *mini_fo_filldir_file;

/* mainly copied from fs/readdir.c */
STATIC int
mini_fo_filldir64(void * __buf, const char * name, int namlen, loff_t offset,
		  ino_t ino, unsigned int d_type)
{
	struct linux_dirent64 * dirent, d;
	struct getdents_callback64 * buf = (struct getdents_callback64 *) __buf;
	int reclen;
	file_t* file = mini_fo_filldir_file;

	/* In theses states we filter meta files in storage (WOL) */
	if(file && (dtopd(file->f_dentry)->state == MODIFIED ||
		    dtopd(file->f_dentry)->state == CREATED ||
		    dtopd(file->f_dentry)->state == DEL_REWRITTEN)) {

		int tmp = strlen(META_FILENAME);
		if(tmp  == namlen) {
			if(!strncmp(name, META_FILENAME, namlen))
				return 0;
		}
	}

	/* check if we are merging the contents of storage and base */
	if(file && dtopd(file->f_dentry)->state == MODIFIED) {
		/* check if we are still reading storage contents, if
		 * yes, we just save the name of the file for duplicate
		 * checking later. */

		if(!ftopd(file)->rd.sto_done) {
			/* put file into ndl list */
			if(ndl_add_entry(&ftopd(file)->rd, name, namlen))
				printk(KERN_CRIT "mini_fo_filldir64: Error adding to ndl.\n");
		} else {
			/* check if file has been deleted */
			if(meta_check_d_entry(file->f_dentry, name, namlen))
				return 0;

			/* do duplicate checking */
			if(ndl_check_entry(&ftopd(file)->rd, name, namlen))
				return 0;
		}
	}


	reclen = ROUND_UP64(NAME_OFFSET(dirent) + namlen + 1);

	buf->error = -EINVAL;/* only used if we fail.. */
	if (reclen > buf->count)
		return -EINVAL;
	dirent = buf->previous;
	if (dirent) {
		d.d_off = offset;
		if(copy_to_user(&dirent->d_off, &d.d_off, sizeof(d.d_off)))
			return -EFAULT;
	}
	dirent = buf->current_dir;
	buf->previous = dirent;
	memset(&d, 0, NAME_OFFSET(&d));
	d.d_ino = ino;
	d.d_reclen = reclen;
	d.d_type = d_type;
	if(copy_to_user(dirent, &d, NAME_OFFSET(&d)) ||
	   copy_to_user(dirent->d_name, name, namlen))
		return -EFAULT;

	put_user(0, dirent->d_name + namlen);
	/* ((char *) dirent) += reclen; */
	dirent = (struct linux_dirent64*) ((char*) dirent + reclen);
	buf->current_dir = dirent;
	buf->count -= reclen;
	return 0;
}


STATIC int
mini_fo_readdir(file_t *file, void *dirent, filldir_t filldir)
{
	int err = 0;/* mk: ??? -ENOTDIR; */
	file_t *hidden_file = NULL;
	file_t *hidden_sto_file = NULL;
	inode_t *inode;
	struct getdents_callback *buf;
	int oldcount;

#if defined(FIST_FILTER_NAME) || defined(FIST_FILTER_SCA)
	struct mini_fo_getdents_callback buf;
#endif /* FIST_FILTER_NAME || FIST_FILTER_SCA */

	buf = (struct getdents_callback *) dirent;
	oldcount = buf->count;
	inode = file->f_dentry->d_inode;
	mini_fo_filldir_file = file;

	ftopd(file)->rd.sto_done = 0;
	do {
		if (ftopd(file) != NULL) {
			if(ftohf2(file)) {
				hidden_sto_file = ftohf2(file);
				err = vfs_readdir(hidden_sto_file, mini_fo_filldir64, dirent);
				file->f_pos = hidden_sto_file->f_pos;
				if (err > 0)
					fist_copy_attr_atime(inode, hidden_sto_file->f_dentry->d_inode);
				/* not finshed yet, we'll be called again */
				if (buf->count != oldcount)
					break;
			}

			ftopd(file)->rd.sto_done = 1;

			if(ftohf(file)) {
				hidden_file = ftohf(file);
				err = vfs_readdir(hidden_file, mini_fo_filldir64, dirent);
				file->f_pos = hidden_file->f_pos;
				if (err > 0)
					fist_copy_attr_atime(inode, hidden_file->f_dentry->d_inode);
			}

		}
	} while (0);

	/* mk:
	 * we need to check if all the directory data has been copied to userspace,
	 * or if we will be called again by userspace to complete the operation.
	 */
	if(buf->count == oldcount) {
		ndl_put_list(&ftopd(file)->rd);
	}

	/* unset this, safe */
	mini_fo_filldir_file = NULL;
	return err;
}


STATIC unsigned int
mini_fo_poll(file_t *file, poll_table *wait)
{
	unsigned int mask = DEFAULT_POLLMASK;
	file_t *hidden_file = NULL;

	if (ftopd(file) != NULL) {
		if(ftohf2(file)) {
			hidden_file = ftohf2(file);
		} else {
			hidden_file = ftohf(file);
		}
	}

	if (!hidden_file->f_op || !hidden_file->f_op->poll)
		goto out;

	mask = hidden_file->f_op->poll(hidden_file, wait);

 out:
	return mask;
}

/* FIST-LITE special version of mmap */
STATIC int
mini_fo_mmap(file_t *file, vm_area_t *vma)
{
	int err = 0;
	file_t *hidden_file = NULL;

	/* fanout capability */
	if (ftopd(file) != NULL) {
		if(ftohf2(file)) {
			hidden_file = ftohf2(file);
		} else {
			hidden_file = ftohf(file);
		}
	}

	ASSERT(hidden_file != NULL);
	ASSERT(hidden_file->f_op != NULL);
	ASSERT(hidden_file->f_op->mmap != NULL);

	vma->vm_file = hidden_file;
	err = hidden_file->f_op->mmap(hidden_file, vma);
	get_file(hidden_file); /* make sure it doesn't get freed on us */
	fput(file);	       /* no need to keep extra ref on ours */

	return err;
}



STATIC int
mini_fo_open(inode_t *inode, file_t *file)
{
	int err = 0;
	int hidden_flags;
	file_t *hidden_file = NULL;
	dentry_t *hidden_dentry = NULL;

	/* fanout stuff */
	file_t *hidden_sto_file = NULL;
	dentry_t *hidden_sto_dentry = NULL;

	__ftopd(file) =
		kmalloc(sizeof(struct mini_fo_file_info), GFP_KERNEL);
	if (!ftopd(file)) {
		err = -ENOMEM;
		goto out;
	}

	/* init the readdir_helper structure */
	INIT_LIST_HEAD(&ftopd(file)->rd.ndl_list);
	ftopd(file)->rd.ndl_size = 0;

	/* In certain paths this could stay uninitalized and cause trouble */
	ftohf(file) = NULL;
	ftohf2(file) = NULL;
	hidden_flags = file->f_flags;

	/* create storage files? */
	if(dtost(file->f_dentry) == UNMODIFIED) {
		if(!IS_WRITE_FLAG(file->f_flags)) {
			hidden_dentry = dtohd(file->f_dentry);
			dget(hidden_dentry);
			/* dentry_open will decrement mnt refcnt if err.
			 * otherwise fput() will do an mntput() for us upon file close. */
			mntget(stopd(inode->i_sb)->hidden_mnt);
			hidden_file = dentry_open(hidden_dentry,
						  stopd(inode->i_sb)->hidden_mnt,
						  hidden_flags);
			if (IS_ERR(hidden_file)) {
				err = PTR_ERR(hidden_file);
				dput(hidden_dentry);
				goto out;
			}
			ftohf(file) = hidden_file;	/* link two files */
			goto out;
		}
		else {
			if(S_ISDIR(file->f_dentry->d_inode->i_mode)) {
				err = dir_unmod_to_mod(file->f_dentry);
			} else
				err = nondir_unmod_to_mod(file->f_dentry, 1);

			if (err) {
				printk("mini_fo_open: ERROR creating storage file.\n");
				goto out;
			}
		}
	}
	hidden_sto_dentry = dtohd2(file->f_dentry);
	dget(hidden_sto_dentry);

	if(dtopd(file->f_dentry)->state == MODIFIED) {
		/* Directorys are special, interpose on both lower level files */
		if(S_ISDIR(itohi(inode)->i_mode)) {
			/* check for invalid file types of lower level files */
			if(!(S_ISDIR(itohi(inode)->i_mode) && S_ISDIR(itohi2(inode)->i_mode))) {
				printk(KERN_CRIT "mini_fo_open: meta data corruption detected.\n");
				dput(hidden_sto_dentry);
				err = -EINVAL;
				goto out;
			}

			/* lower level directorys are ok, open the base file */
			hidden_dentry = dtohd(file->f_dentry);
			dget(hidden_dentry);

			mntget(stopd(inode->i_sb)->hidden_mnt);
			hidden_file = dentry_open(hidden_dentry,
						  stopd(inode->i_sb)->hidden_mnt,
						  hidden_flags);
			if (IS_ERR(hidden_file)) {
				err = PTR_ERR(hidden_file);
				dput(hidden_dentry);
				dput(hidden_sto_dentry);
				goto out;
			}
			ftohf(file) = hidden_file; /* link the two files */
		}
	}

	if(!exists_in_storage(file->f_dentry)) {
		printk(KERN_CRIT "mini_fo_open: invalid file state detected.\n");
		err = -EINVAL;
		dput(hidden_sto_dentry);

		/* If the base file has been opened, we need to close it here */
		if(ftohf(file)) {
			if (hidden_file->f_op && hidden_file->f_op->flush)
				hidden_file->f_op->flush(hidden_file);
			dput(hidden_dentry);
		}
		goto out;
	}

	/* ok, now we can safely open the storage file */
	mntget(stopd(inode->i_sb)->hidden_mnt2);
	hidden_sto_file = dentry_open(hidden_sto_dentry,
				      stopd(inode->i_sb)->hidden_mnt2,
				      hidden_flags);

	/* dentry_open dputs the dentry if it fails */
	if (IS_ERR(hidden_sto_file)) {
		err = PTR_ERR(hidden_sto_file);
		/* close base file if open */
		if(ftohf(file)) {
			if (hidden_file->f_op && hidden_file->f_op->flush)
				hidden_file->f_op->flush(hidden_file);
			dput(hidden_dentry);
		}
		goto out;
	}
	ftohf2(file) = hidden_sto_file; /* link storage file */

 out:
	if (err < 0 && ftopd(file)) {
		kfree(ftopd(file));
	}
	return err;
}

STATIC int
mini_fo_flush(file_t *file)
{
	int err1 = 0;		/* assume ok (see open.c:close_fp) */
	int err2 = 0;
	file_t *hidden_file = NULL;

	check_mini_fo_file(file);

	/* mk: we don't do any state checking here, as its not worth the time.
	 * Just flush the lower level files if they exist.
	 */
	if(ftopd(file) != NULL) {
		if(ftohf(file) != NULL) {
			hidden_file = ftohf(file);
			if (hidden_file->f_op && hidden_file->f_op->flush)
				err1 = hidden_file->f_op->flush(hidden_file);
		}
		if(ftohf2(file) != NULL) {
			hidden_file = ftohf2(file);
			if (hidden_file->f_op && hidden_file->f_op->flush)
				err2 = hidden_file->f_op->flush(hidden_file);
		}
	}
	return (err1 | err2);
}


STATIC int
mini_fo_release(inode_t *inode, file_t *file)
{
	int err = 0;
	file_t *hidden_file = NULL;

	if (ftopd(file) != NULL) {
		if(ftohf(file)) {
			hidden_file = ftohf(file);
			fput(hidden_file);
		}
		if(ftohf2(file)) {
			hidden_file = ftohf2(file);
			fput(hidden_file);
		}
		kfree(ftopd(file));
	}
	return err;
}

STATIC int
mini_fo_fsync(file_t *file, dentry_t *dentry, int datasync)
{
	int err1 = 0;
	int err2 = 0;
	file_t *hidden_file = NULL;
	dentry_t *hidden_dentry;

	check_mini_fo_file(file);

	if ((hidden_file = ftohf(file)) != NULL) {
		hidden_dentry = dtohd(dentry);
		if (hidden_file->f_op && hidden_file->f_op->fsync) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
			mutex_lock(&hidden_dentry->d_inode->i_mutex);
#else
			down(&hidden_dentry->d_inode->i_sem);
#endif
			err1 = hidden_file->f_op->fsync(hidden_file, hidden_dentry, datasync);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
			mutex_unlock(&hidden_dentry->d_inode->i_mutex);
#else
			up(&hidden_dentry->d_inode->i_sem);
#endif
		}
	}

	if ((hidden_file = ftohf2(file)) != NULL) {
		hidden_dentry = dtohd2(dentry);
		if (hidden_file->f_op && hidden_file->f_op->fsync) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
			mutex_lock(&hidden_dentry->d_inode->i_mutex);
#else
			down(&hidden_dentry->d_inode->i_sem);
#endif
			err2 = hidden_file->f_op->fsync(hidden_file, hidden_dentry, datasync);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
			mutex_unlock(&hidden_dentry->d_inode->i_mutex);
#else
			up(&hidden_dentry->d_inode->i_sem);
#endif
		}
	}
	else
		goto err;

err:
	return (err1 || err2);
}


STATIC int
mini_fo_fasync(int fd, file_t *file, int flag)
{
	int err1 = 0;
	int err2 = 0;

	file_t *hidden_file = NULL;

	check_mini_fo_file(file);

	if((hidden_file = ftohf(file)) != NULL) {
		err1 = hidden_file->f_op->fasync(fd, hidden_file, flag);
	}
	if((hidden_file = ftohf2(file)) != NULL) {
		err2 = hidden_file->f_op->fasync(fd, hidden_file, flag);
	}

	return (err1 || err2);
}

struct file_operations mini_fo_dir_fops =
	{
		llseek:	mini_fo_llseek,
		read:	mini_fo_read,
		write:	mini_fo_write,
		readdir: mini_fo_readdir,
		poll:	mini_fo_poll,
		/* ioctl:	mini_fo_ioctl, */
		mmap:	mini_fo_mmap,
		open:	mini_fo_open,
		flush:	mini_fo_flush,
		release: mini_fo_release,
		fsync:	mini_fo_fsync,
		fasync:	mini_fo_fasync,
		/* not needed lock:	mini_fo_lock, */
		/* not needed: readv */
		/* not needed: writev */
		/* not implemented: sendpage */
		/* not implemented: get_unmapped_area */
	};

struct file_operations mini_fo_main_fops =
	{
		llseek:	mini_fo_llseek,
		read:	mini_fo_read,
		write:	mini_fo_write,
		readdir: mini_fo_readdir,
		poll:	mini_fo_poll,
		/* ioctl:	mini_fo_ioctl, */
		mmap:	mini_fo_mmap,
		open:	mini_fo_open,
		flush:	mini_fo_flush,
		release: mini_fo_release,
		fsync:	mini_fo_fsync,
		fasync:	mini_fo_fasync,
		/* not needed: lock:	mini_fo_lock, */
		/* not needed: readv */
		/* not needed: writev */
		/* not implemented: sendpage */
		/* not implemented: get_unmapped_area */
	};
