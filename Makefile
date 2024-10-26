export PATH := $(shell pwd)/toolchain/bin:$(PATH)

export ARCH ?= i686
export CC = $(ARCH)-elf-gcc
export LD = $(ARCH)-elf-gcc
export AS = $(ARCH)-elf-as

build:
	make -C kernel

clean:
	make -C kernel clean

run: build
	qemu-system-i386 -m 128			\
		-kernel kernel/kiwi.elf 	\
		-serial stdio				\
		-display sdl 				\
		-smp 1

config-clangd: clean
	bear -- make build

help:
	@echo "Welcome to the Makefile for the Kiwi kernel"
	@echo ""
	@echo "If this is your first time building the kernel, run './scripts/toolchain.sh' to install the toolchain"
	@echo "Once you have the toolchain installed, you can run 'make' to build the kernel"
	@echo "Once the kernel and userland are built, you can run 'make run' to run the kernel in QEMU"
