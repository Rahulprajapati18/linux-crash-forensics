obj-m += crashmon.o

KERNEL_DIR := $(HOME)/wsl-kernel

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
