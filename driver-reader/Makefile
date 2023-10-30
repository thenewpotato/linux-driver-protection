obj-m += reader.o
all:
	make -j -C ../linux-5.15.92 M="$(PWD)" modules
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
