.PHONY: initfs qemu

initfs:
	(cd initfs; find . | cpio -ov --format=newc | gzip -9 > ../initramfz)

qemu:
#	qemu_system-x86_64 -kernel linux-5.15.92/arch/x86/boot/bzImage -boot c -m 2049M -hda buildroot/output/images/rootfs.ext4 -append "root=/dev/sda rw console=ttyS0,115200 acpi=off nokaslr" -serial stdio -display none
	qemu-system-x86_64 -kernel linux-5.15.92/arch/x86/boot/bzImage -initrd initramfz -m 1048 -s -nographic -append 'root=/dev/sda rw console=ttyS0, acpi=off nokaslr' -S

clean:
	rm -f initramfz
