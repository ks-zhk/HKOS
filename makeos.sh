nasm -o build/mbr.bin boot/mbr.S
nasm -o build/loader.bin boot/loader.S
gcc -c -fno-builtin -m32 -I device/c/ -I lib/ -I lib/kernel/ -o build/timer.o device/c/timer.c
gcc -c -fno-builtin -m32 -I lib/kernel/ -I lib/  -o build/main.o lib/kernel/main.c
nasm -f elf -o build/print.o lib/kernel/print.S
nasm -f elf -o build/kernel.o lib/kernel/kernel.S
gcc -m32 -fno-builtin -I lib/kernel/ -m32 -I lib/ -c  -o build/interrupt.o lib/kernel/interrupt.c -m32
gcc -m32 -fno-builtin -I lib/kernel/ -I lib/ -c  -o build/init.o lib/kernel/init.c -m32

ld build/main.o build/init.o build/interrupt.o build/print.o build/timer.o build/kernel.o  -Ttext 0xc0001500 -e main -o build/kernel.bin -m elf_i386

dd if=build/mbr.bin of=/home/ksdeer/apps/bochs/bochs_2_6_2/bin/hd60M.img bs=512 count=1 conv=notrunc
dd if=build/loader.bin of=/home/ksdeer/apps/bochs/bochs_2_6_2/bin/hd60M.img bs=512 count=4 seek=2 conv=notrunc
dd if=build/kernel.bin of=/home/ksdeer/apps/bochs/bochs_2_6_2/bin/hd60M.img bs=512 count=200 seek=9 conv=notrunc

