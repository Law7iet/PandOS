# Compilatore e linker
XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld

# Path di uMPS3
ifneq ($(wildcard /usr/bin/umps3),)
	UMPS3_DIR_PREFIX = /usr
else
	UMPS3_DIR_PREFIX = /usr/local
endif
UMPS3_DATA_DIR = $(UMPS3_DIR_PREFIX)/share/umps3
UMPS3_INCLUDE_DIR = $(UMPS3_DIR_PREFIX)/include/umps3

# Opzioni del compialtore
CFLAGS_LANG = -ffreestanding -ansi
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS3_INCLUDE_DIR) -I./headers -Wall -O0

# Opzioni del linker
LDFLAGS = -G 0 -nostdlib -T $(UMPS3_DATA_DIR)/umpscore.ldscript

# Path di ricerca per il file crtso.S e libumps.S
VPATH = $(UMPS3_DATA_DIR)

# Altre variabili
OBJFILES = $(OBJDIR)p1test.o $(OBJDIR)asl.o $(OBJDIR)pcb.o $(OBJDIR)crtso.o $(OBJDIR)libumps.o
SRCDDIR = src/
OBJDIR = obj/

.PHONY : all clean

all : kernel.core.umps

kernel.core.umps : kernel
	umps3-elf2umps -k kernel

kernel : p1test.o asl.o pcb.o crtso.o libumps.o
	$(LD) -o kernel $(OBJFILES) $(LDFLAGS)

crtso.o : 
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(UMPS3_DATA_DIR)/crtso.S

libumps.o :
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(UMPS3_DATA_DIR)/libumps.S

%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCIDR)$<

clean :
	-rm -f kernel kernel.* obj/*.o