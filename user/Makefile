ifndef TOOLPREFIX
TOOLPREFIX := $(shell if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-elf-'; \
	elif riscv64-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-linux-gnu-'; \
	elif riscv64-unknown-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-linux-gnu-'; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find a riscv64 version of GCC/binutils." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

LDFLAGS = -z max-page-size=4096

clean: 
	rm -f initcode.asm initcode.d initcode.h initcode.o initcode.out initcode.txt usys.d usys.o usys.S

init:
	perl usys.pl > usys.S
	$(CC) $(CFLAGS) -I . -march=rv64g -nostdinc -c initcode.c -o initcode.o
	$(CC) $(CFLAGS) -march=rv64g -nostdinc -I. -c usys.S -o usys.o
	$(CC) $(CFLAGS) -I . -march=rv64g -nostdinc -c printf.c -o printf.o
	$(CC) $(CFLAGS) -I . -march=rv64g -nostdinc -c umalloc.c -o umalloc.o
	$(LD) $(LDFLAGS) -T user.ld -N -e main -Ttext 0 -o initcode.out initcode.o usys.o printf.o umalloc.o
	$(OBJDUMP) -S initcode.out > initcode.asm
	xxd -i initcode.out > ./initcode.h
	./parse_elf
	# rm -f initcode initcode.d initcode.o initcode.out
