TARGET = kiwi.elf

CFLAGS += -O2 -flto -march=i686
CFLAGS += -c -ffreestanding -fsanitize=undefined
CFLAGS += -masm=intel
CFLAGS += -std=gnu23
CFLAGS += -I include
CFLAGS += -W -Wall -Wextra -Wshadow -Wredundant-decls -Wno-unused-function
CFLAGS += -fdata-sections -ffunction-sections

# AS flags
ASFLAGS += -c

# LD flags
LDFLAGS += -flto -fsanitize=undefined
LDFLAGS += -ffreestanding -nostdlib
LDFLAGS += -T arch/config/link.ld 
LDFLAGS += -Wl,--gc-sections

# Source files
ASFILES = $(shell find . -type f -name "*.asm")
CFILES = $(shell find . -type f -name "*.c")

# Objects files
OBJFILES += $(ASFILES:.asm=.o)
OBJFILES += $(CFILES:.c=.o)

.PHONY: all clean

all: $(TARGET) 

$(TARGET): $(OBJFILES)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $^ -o $@

clean:
	rm -f $(OBJFILES) $(TARGET)
