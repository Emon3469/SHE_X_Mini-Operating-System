C_SOURCES = $(wildcard kernel/*/*.c) $(wildcard kernal/*/*.c)
CXX_SOURCES = $(wildcard kernel/*/*.cpp) $(wildcard kernal/*/*.cpp)
ASM_SOURCES = $(wildcard kernel/*/*.asm) $(wildcard kernal/*/*.asm)
C_OBJECTS = ${C_SOURCES:.c=.o}
CXX_OBJECTS = ${CXX_SOURCES:.cpp=.o}
ASM_OBJECTS = ${ASM_SOURCES:.asm=.o}

.PHONY : all assemble run clean

all: run

run : assemble
	qemu-system-i386 -drive format=raw,file=disk.img  -monitor stdio

debug: assemble
	qemu-system-i386 -s -hda disk.img &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel/kernel.elf" -ex "b kmain" -ex "continue"

assemble: disk.img kernel.bin stage1.bin stage2.bin 
	dd if=stage1.bin of=disk.img bs=1 count=3 seek=0 skip=0 conv=notrunc
	dd if=stage1.bin of=disk.img bs=1 count=451 seek=62 skip=62 conv=notrunc
	# copy stage2 and kernel into the FAT image; mcopy syntax varies — keep minimal args
	mcopy -i disk.img stage2.bin kernel.bin :: -D


kernel.bin : kernel/kernel.elf  
	objcopy -O binary $^ $@
	chmod -x $@

#You can use the --print-map option to look at what the linker does

kernel/kernel.elf : $(C_OBJECTS) $(CXX_OBJECTS) $(ASM_OBJECTS)
	i686-elf-ld  $^ -T kernal/linker.ld -e kmain -o $@ 
	chmod -x $@


%.o : %.c
	i686-elf-gcc -Ikernel/include -Ikernal/include -ffreestanding $< -c -o $@ -Wall -Werror -g

%.o : %.cpp
	i686-elf-g++ -Ikernel/include -Ikernal/include -ffreestanding $< -c -o $@ -Wall -Werror -g -std=gnu++11

%.o : %.asm
	nasm $< -o $@ -f elf32

stage1.bin : boot/stage1/stage1.asm
	nasm $^ -f bin -o $@
stage2.bin: boot/stage2/stage2.asm
	nasm $^ -f bin -o $@
	
disk.img: 
	truncate $@ -s 1M
	mkfs.vfat -F12 -S512 -s1 $@
	
clean :
	rm $(C_OBJECTS) $(ASM_OBJECTS) *.bin

.PHONY : tools-check
tools-check:
	@which i686-elf-gcc >/dev/null 2>&1 || echo "Missing i686-elf-gcc"
	@which i686-elf-ld >/dev/null 2>&1 || echo "Missing i686-elf-ld"
	@which i686-elf-g++ >/dev/null 2>&1 || echo "Missing i686-elf-g++"
	@which nasm >/dev/null 2>&1 || echo "Missing nasm"
	@which qemu-system-i386 >/dev/null 2>&1 || echo "Missing qemu-system-i386"
	@which mcopy >/dev/null 2>&1 || echo "Missing mcopy"
	@which mkfs.fat >/dev/null 2>&1 || which mkfs.vfat >/dev/null 2>&1 || echo "Missing mkfs.fat/mkfs.vfat"
	@which dd >/dev/null 2>&1 || echo "Missing dd"
	@which truncate >/dev/null 2>&1 || echo "Missing truncate"
