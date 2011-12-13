EXTRA_CFLAGS := -Werror -I../../bcmdrivers/opensource/include/bcm963xx -I../../shared/opensource/include/bcm963xx

obj-$(CONFIG_OPROFILE) += oprofile.o

DRIVER_OBJS = $(addprefix ../../../drivers/oprofile/, \
		oprof.o cpu_buffer.o buffer_sync.o \
		event_buffer.o oprofile_files.o \
		oprofilefs.o oprofile_stats.o \
		timer_int.o )

oprofile-y				:= $(DRIVER_OBJS) common.o

oprofile-$(CONFIG_MIPS_BRCM)		+= op_model_brcm_timer.o

