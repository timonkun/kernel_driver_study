# If KERNELRELEASE is defined, we've been invoked from the
# # kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)

obj-m := hello.o 
#  # Otherwise we were called directly from the command
# line; invoke the kernel build system.
else

KERNELDIR ?= ../../linux-3.6.3
PWD := $(shell pwd) 
all:
	make -C $(KERNELDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-

clean:
	rm -f *.o *.ko *.mod.c *.mod.o *.symvers
endif 
