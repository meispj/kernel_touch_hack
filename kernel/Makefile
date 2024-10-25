MODULE_NAME=Touch
RESMAN_CORE_OBJS := touch.o
RESMAN_GLUE_OBJS :=
ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULE_NAME).o
	$(MODULE_NAME)-objs := $(RESMAN_GLUE_OBJS) $(RESMAN_CORE_OBJS)
else
	ifeq ($(shell echo $(KERNEL_VERSION) | grep -i coloros),)
		KDIR := /home/$(KERNEL_VERSION)/out
	else
		KDIR := /home/$(KERNEL_VERSION)/kernel/kernel/out
	endif

all:
	make -j$(nproc) -C $(KDIR) M=$(shell pwd) LLVM=1 LLVM_IAS=1 ARCH=arm64 SUBARCH=arm64 CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-gnu- CC=clang LD=ld.lld OBJCOPY=llvm-objcopy OBJDUMP=llvm-objdump NM=llvm-nm STRIP=llvm-strip modules
	rm -rf .*.mod.cmd *.o .*.ko.cmd .*.o.cmd *.mod.* *.mod.cmd *.mod *.mod.c *.symvers *.order .tmp_versions
	llvm-strip --strip-debug $(MODULE_NAME).ko
	cp $(MODULE_NAME).ko /home/user

clean:
	rm -rf *.ko *.o .*.ko.cmd .*.o.cmd *.mod.o *.mod.c *.symvers *.order .tmp_versions
endif
