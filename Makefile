export PATH := $(shell pwd)/toolchain/bin:$(PATH)

export ARCH ?= i686
export CC = $(ARCH)-elf-gcc
export LD = $(ARCH)-elf-gcc
export AS = $(ARCH)-elf-as

.PHONY: build clean run config-clangd help

build:
	make -C kernel

clean:
	make -C kernel clean

# Run the kernel in QEMU
run: build
	qemu-system-i386 -m 128			\
		-kernel kernel/kiwi.elf 	\
		-serial stdio				\
		-smp 1

# Generate a compile_commands.json for clangd by compiling the project
# with bear that intercepts the compiler calls and generates the json file.
# This is useful for clangd to provide code completion and other features.
config-clangd: clean
	bear -- make build

# Print a help message
help:
	@echo "Welcome to the Makefile for the Kiwi kernel"
	@echo ""
	@echo "If this is your first time building the kernel, run './scripts/toolchain.sh' to install the necessary toolchain"
	@echo "Once you have the toolchain installed, you can run 'make' to build the kernel"
	@echo "Once the kernel and userland are built, you can run 'make run' to run the kernel in QEMU"
