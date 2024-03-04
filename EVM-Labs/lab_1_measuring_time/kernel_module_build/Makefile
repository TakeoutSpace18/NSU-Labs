obj-m += cpu_time_measure.o
cpu_time_measure-y += ../src/measurable_code.o ../src/rdtsc_kernel_module.o

ccflags-y := -msse2

PWD := $(CURDIR)
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean