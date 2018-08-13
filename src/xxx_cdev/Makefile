
ifeq ($(KERNELRELEASE),)

include Makefile.inc.mk
ifeq ($(my-src-dir),)
$(error "Invalid src dir.")
endif

.PHONY: all
all: modules

#
# CMD_MAKE_LINUX
# A wrapper to run make on linux kernel (support cross compile)
# 1: Linux kernel source directory
# 2: This module directory
# 3: Kernel arch
# 4: Cross compiler prefix
# 5: Verbose level
# 6: Makefile target
#
CMD_MAKE_LINUX = if [ ! -d "$(1)" -o ! -s "$(1)/.config" ]; then echo " * ERROR: Plz make sure kernel config '$(1)/.config' is ready" && exit 255; else $(MAKE) -C "$(1)" M=$(2) $(if $(3),ARCH=$(3)) $(if $(4),CROSS_COMPILE=$(4)) $(if $(5),V=$(5)) $(6) ; fi;

modules:
	$(call CMD_MAKE_LINUX,$(my-linux-dir),$(my-src-dir),$(my-arch),$(my-cross-pfx),0,modules)

clean:
	-$(call CMD_MAKE_LINUX,$(my-linux-dir),$(my-src-dir),$(my-arch),$(my-cross-pfx),0,clean)

else

ifeq ($(M),)
$(error "Invalid symbol M=$(M)")
endif

include $(M)/Makefile.inc.mk

ccflags-y += $(my-extra-cflags)

obj-m := $(my-kmod-name).o
$(my-kmod-name)-objs := $(my-obj-y)

endif

#;
