obj-m += datastore.o
obj-m += reader.o
obj-m += stringstore.o
all:
	make -j -C ../linux-5.15.92 M="$(PWD)" modules
clean:
	make -j -C ../linux-5.15.92 M="$(PWD)" clean
