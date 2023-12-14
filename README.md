# Linux Module & Driver Protection

This is the source code repository for "Protecting Linux Device Drivers and Loadable Kernel Modules from Untrusted Operating Systems", an undergraduate thesis in Computer Science at Yale University.

More details on the project can be accessed on [the project site](https://zoo.cs.yale.edu/classes/cs490/23-24a/wang.tiger.jw2723/) (Yale network only).

## Building

First build our custom Linux kernel by navigating to to the `linux-5.15.92` directory and running (this may take a while)
```
make -j 4
```

Change back to the project root directory and build the initram file (this line takes the contents of initfs and compresses them into `initramfz`)
```
make initfs
```

If you make changes to the sample drivers, you should also build the drivers. This step is not necessary if you do not make driver changes. After changing into the `drivers` directory, simply
```
make
```

## Running

After building the kernel, run it using the QEMU emulator. In the project root directory, run
```
make qemu
```

QEMU will wait for a GDB debugger instance to attach. In a separate terminal window, change into the `linux-5.15.92` directory and run
```
gdb vmlinux
```

After GDB finishes loading symbols, attach it to the kernel. In the GDB command line interface, execute
```
target remote:1234
```

Then uninterrupt kernel execution by running `c` or `continue` in the GDB command line.

## Testing

### datastore (Generic LKM)

In the custom kernel running in QEMU, run
```
insmod datastore.ko
```

To read and write values into `datastore`, run
```
insmod reader.ko
```

### stringstore (Device Driver)

In the custom kernel running in QEMU, run
```
insmod stringstore.ko
```

(To turn off driver protection, substitute the above command with `insmod stringstore_copy.ko`)

Manually initialize the device node
```
mknod /dev/stringstore c [MAJOR] 0
```

To read from `stringstore`, run
```
head -c10 /dev/stringstore
```

To write to `stringstore`, run
```
echo "Bye!" > /dev/stringstore
```