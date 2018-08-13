ifeq ($(KERNELRELEASE),)
#
# dir
#
my-src-dir = $(CURDIR)

my-linux-dir ?= /lib/modules/$(shell uname -r)/build
my-cross-pfx ?=
my-arch ?=

else
#
# local cflags to apply on all obj
#
my-extra-cflags ?=
my-extra-cflags +=

#
# module
# 
my-kmod-name := xxx

my-obj-y :=
my-obj-y += xxx_main.o
my-obj-y += xxx_cdev.o

endif

#;
