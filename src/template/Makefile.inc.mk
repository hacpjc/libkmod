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
my-kmod-name := template

my-obj-y :=
my-obj-y += template_main.o

endif

#;
