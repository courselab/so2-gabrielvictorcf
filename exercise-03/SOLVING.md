# Solving exercise-03 - mydos

The first few steps, 1 through 5, are mostly setup. Of note, we edited the `.ld` linker script to remove
some static dependencies and added a `make disk.img` rule to our makefile, using `dd` to generate a
disk image the same size as a floppy `1.44Mb`.

Then, steps 6+7 require us to create a custom bootable disk image. The solution for this relies on the knowing about
the Master Boot Record.
1. The BIOS will check memory sector 0, the first 512 bytes, of the image
2. The magic number should be `0x55` at offset 510 and `0xaa` at offsite 511

This means that since our filesystem header is small (16 bytes), the same as our bootloaders address, we can
replace the bootloaders header with this and leave our fs header at sector 0 without impacting execution.

That explains the `make mount` rule in the makefile.

For step 8, we just had to
1. Fetch fs header (already in ram)
2. Load directory into ram (offset found using boot sector len from header)
3. iterate and list the files

For step 9, we create a static memory region, which we can call userspace, in the ld script.
Then:
1. copy it into filesystem
2. figure out, by name (query directory in fs header), it's disk memory location
3. load into userspace
4. jump into it

Additionally, we also had to make a "fake" call into the program's entry function, by
pushing the ip as instructed in the readme.

Finally, for step 10, we expose the `read` syscall through a `gets` external function and
call it in our user program. For this, the actual library that defines syscalls must be
linked to the program statically.

P.S: could not debug, but when user program calls into syscall, program is falling into infinite
loop, i suspect this may be due to sharing stack memory region, but not sure in the present moment.