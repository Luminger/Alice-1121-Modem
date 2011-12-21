/*
 * $Id$
 * Copyright (C) 2006  Florian Fainelli <florian@openwrt.org>
 * 			Mike Albon <malbon@openwrt.org>
 * Copyright (C) $Date$  $Author$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* This is the BCM963xx flash map driver, in its actual state it only supports BCM96348 devices
 * this driver is able to manage both bootloader we found on these boards : CFE and RedBoot
 *
 * RedBoot :
 *  - this bootloader allows us to parse partitions and therefore deduce the MTD partition table
 *
 * CFE :
 *   - CFE partitionning can be detected as for BCM947xx devices 
 *
 */

/*
 * Michael Finsterbusch: modifyed this module to add NVRAM partition 
 *                       and delete all RedBoot code parts
 */

#include <asm/io.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/vmalloc.h>
#include <board.h>

#define WINDOW_ADDR 0x1FC00000         /* Real address of the flash */
#define WINDOW_SIZE 0x400000           /* Size of flash */
#define BUSWIDTH 2                     /* Buswidth */
#define EXTENDED_SIZE 0xBFC00000       /* Extended flash address */
#define IMAGE_LEN 10                   /* Length of Length Field */
#define ADDRESS_LEN 12                 /* Length of Address field */
#define ROUNDUP(x, y) ((((x)+((y)-1))/(y))*(y))

static struct mtd_partition *parsed_parts;

static void __exit bcm963xx_mtd_cleanup(void);

static struct mtd_info *bcm963xx_mtd_info;

static struct map_info bcm963xx_map = {
       .name = "bcm963xx",
       .size = WINDOW_SIZE,
       .bankwidth = BUSWIDTH,
       .phys = WINDOW_ADDR,
};


int parse_cfe_partitions( struct mtd_info *master, struct mtd_partition **pparts)
{
       int nrparts = 4, curpart = 0; // CFE and NVRAM always present. linux and all flash, too
       struct bcm963xx_cfe_map {
       unsigned char tagVersion[4];                            // Version of the image tag
       unsigned char sig_1[20];                                // Company Line 1
       unsigned char sig_2[14];                                // Company Line 2
       unsigned char chipid[6];                                        // Chip this image is for
       unsigned char boardid[16];                              // Board name
       unsigned char bigEndian[2];                             // Map endianness -- 1 BE 0 LE
       unsigned char totalLength[IMAGE_LEN];           //Total length of image
       unsigned char cfeAddress[ADDRESS_LEN];  // Address in memory of CFE
       unsigned char cfeLength[IMAGE_LEN];             // Size of CFE
       unsigned char rootAddress[ADDRESS_LEN];         // Address in memory of rootfs
       unsigned char rootLength[IMAGE_LEN];            // Size of rootfs
       unsigned char kernelAddress[ADDRESS_LEN];       // Address in memory of kernel
       unsigned char kernelLength[IMAGE_LEN];  // Size of kernel
       unsigned char imageSequence[4];   // incrments everytime an image is flashed
       unsigned char reserved[90];        // reserved for later use
       unsigned char imageValidationToken[20];// image validation token - can be crc, md5, sha;  for
                                                 // now will be 4 unsigned char crc
       unsigned char tagValidationToken[20]; // validation token for tag(from signiture_1 to end of // mageValidationToken)

       } *buf;
       struct mtd_partition *parts;
       int ret;
       size_t retlen;
       unsigned int rootfsaddr, kerneladdr, spareaddr;
       unsigned int rootfslen, kernellen, sparelen, totallen;
       int namelen = 0;
       int i;
       // Allocate memory for buffer
       buf = vmalloc(sizeof(struct bcm963xx_cfe_map));

       if (!buf)
               return -ENOMEM;

       // Get the tag
       // 0x10000
       ret = master->read(master,master->erasesize,sizeof(struct bcm963xx_cfe_map), &retlen, (void *)buf);
       if (retlen != sizeof(struct bcm963xx_cfe_map)){
               vfree(buf);
               return -EIO;
       };
       printk("bcm963xx: CFE boot tag found with version %s and board type %s.\n",buf->tagVersion,buf->boardid);
       // Get the values and calculate
       sscanf(buf->rootAddress,"%u", &rootfsaddr);
       rootfsaddr = rootfsaddr - EXTENDED_SIZE;
       sscanf(buf->rootLength, "%u", &rootfslen);
       sscanf(buf->kernelAddress, "%u", &kerneladdr);
       kerneladdr = kerneladdr - EXTENDED_SIZE;
       sscanf(buf->kernelLength, "%u", &kernellen);
       sscanf(buf->totalLength, "%u", &totallen);
       spareaddr = ROUNDUP(totallen,master->erasesize) + master->erasesize;
// w NVRAM 
     sparelen = master->size - spareaddr - master->erasesize;
// w/o NVRAM
   //    sparelen = master->size - spareaddr;
       // Determine number of partitions
       namelen = 8;
       if (rootfslen > 0){
               nrparts++;
               namelen =+ 6;
       };
       if (kernellen > 0){
               nrparts++;
               namelen =+ 6;
       };
       if (sparelen > 0){
               nrparts++;
               namelen =+ 6;
       };
       
       // Ask kernel for more memory.
       parts = kmalloc(sizeof(*parts)*nrparts+10*nrparts, GFP_KERNEL);
       if (!parts){
               vfree(buf);
               return -ENOMEM;
       };
       memset(parts,0,sizeof(*parts)*nrparts+10*nrparts);

	// Start building partition list
       parts[curpart].name = "CFE";
       parts[curpart].offset = 0;
       parts[curpart].size = master->erasesize;
       curpart++;
       
       //parts[curpart].name = "BRCMLAYOUT";
       //parts[curpart].offset = master->erasesize;
       //parts[curpart].size = 256;
       //curpart++;

       if (kernellen > 0){
               parts[curpart].name = "kernel";
               parts[curpart].offset = kerneladdr;
               parts[curpart].size = kernellen;
               curpart++;
       };
       if (rootfslen > 0){
               parts[curpart].name = "rootfs";
               parts[curpart].offset = rootfsaddr;
               parts[curpart].size = rootfslen;
               curpart++;
       };
       if (sparelen > 0){
               parts[curpart].name = "data";
               parts[curpart].offset = spareaddr;
               parts[curpart].size = sparelen;
               curpart++;
       };
      
       parts[curpart].name = "NVRAM";
       parts[curpart].offset = master->size - master->erasesize;
       parts[curpart].size = master->erasesize;
       curpart++;

       parts[curpart].name = "linux";
       parts[curpart].offset = master->erasesize;
       parts[curpart].size = WINDOW_SIZE - master->erasesize;
	   
       nrparts = curpart+1;
	   for (i = 0; i < nrparts; i++) {
          printk("bcm963xx: Partition %d is %s offset %x and length %x\n", i, parts[i].name, parts[i].offset, parts[i].size);
       }
       *pparts = parts;
       vfree(buf);
       return nrparts;
};

static int bcm963xx_detect_cfe(struct mtd_info *master)
{
       int idoffset = 0x4e0;
       static char idstring[8] = "CFE1CFE1";
       char buf[8];
       int ret;
       size_t retlen;

       ret = master->read(master, idoffset, 8, &retlen, (void *)buf);
       printk("bcm963xx: Read Signature value of %s\n", buf);
       return strcmp(idstring,buf);
}

static int __init bcm963xx_mtd_init(void)
{
       printk("bcm963xx: 0x%08x at 0x%08x\n", WINDOW_SIZE, WINDOW_ADDR);
       bcm963xx_map.virt = ioremap(WINDOW_ADDR, WINDOW_SIZE);

       if (!bcm963xx_map.virt) {
               printk("bcm963xx: Failed to ioremap\n");
               return -EIO;
       }

       simple_map_init(&bcm963xx_map);

       //"map_rom"
       bcm963xx_mtd_info = do_map_probe("cfi_probe", &bcm963xx_map);

       if (bcm963xx_mtd_info) {
               bcm963xx_mtd_info->owner = THIS_MODULE;

               if (bcm963xx_detect_cfe(bcm963xx_mtd_info) == 0)
               {
                       int parsed_nr_parts = 0;
                       char * part_type;
                       printk("bcm963xx: CFE bootloader detected\n");
                       if (parsed_nr_parts == 0) {
                               int ret = parse_cfe_partitions(bcm963xx_mtd_info, &parsed_parts);
                               if (ret > 0) {
                                       part_type = "CFE";
                                       parsed_nr_parts = ret;
                               }
                       }
                       add_mtd_partitions(bcm963xx_mtd_info, parsed_parts, parsed_nr_parts);
                       return 0;
               }
       }
       iounmap(bcm963xx_map.virt);
       return -ENXIO;
}

static void __exit bcm963xx_mtd_cleanup(void)
{
       if (bcm963xx_mtd_info) {
               del_mtd_partitions(bcm963xx_mtd_info);
               map_destroy(bcm963xx_mtd_info);
       }

       if (bcm963xx_map.virt) {
               iounmap(bcm963xx_map.virt);
               bcm963xx_map.virt = 0;
       }
}

module_init(bcm963xx_mtd_init);
module_exit(bcm963xx_mtd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org> Mike Albon <malbon@openwrt.org>");
