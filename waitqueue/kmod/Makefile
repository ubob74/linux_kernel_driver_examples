obj-m += waitqueue_kmod.o
KERNEL_SRC := /usr/src/linux
all:
	make -C $(KERNEL_SRC) M=$(PWD) modules
clean:
	make -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f *~ *.o
