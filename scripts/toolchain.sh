#!/bin/bash
set -e

if [ -d toolchain ]; then
	echo "Toolchain already installed" 1>&2;
	echo "If you want to reinstall it, remove the toolchain directory" 1>&2;
	exit 1;
fi
	
echo "Creating required directories"
mkdir -p bin
mkdir -p toolchain/src
mkdir -p toolchain/build/binutils-2.42
mkdir -p toolchain/build/gcc-14.1.0
	
echo "Downloading binutils-2.42.tar.gz"
wget https://mirror.ibcp.fr/pub/gnu/binutils/binutils-2.42.tar.xz \
	-O toolchain/src/binutils-2.42.tar.xz

echo "Downloading gcc-14.1.0.tar.gz"	
wget https://mirror.ibcp.fr/pub/gnu/gcc/gcc-14.1.0/gcc-14.1.0.tar.xz \
	-O toolchain/src/gcc-14.1.0.tar.xz
	
echo "Extracting binutils-2.42.tar.gz"
tar -xf toolchain/src/binutils-2.42.tar.xz -C toolchain/src
echo "Extracting gcc-14.1.0.tar.gz"
tar -xf toolchain/src/gcc-14.1.0.tar.xz -C toolchain/src

echo "Downloading gcc prerequisites"
cd toolchain/src/gcc-14.1.0
./contrib/download_prerequisites
cd ../../..
	
export TARGET=i686-elf
export PREFIX="$(pwd)/toolchain/"
		
echo "Building binutils"
cd toolchain/build/binutils-2.42
../../src/binutils-2.42/configure 	\
  	--target="$TARGET"	  	\
	--prefix="$PREFIX"	  	\
	--with-sysroot			\
	--disable-werror
	
make -j $(nproc)
make install-strip
	
echo "Building gcc, this may take a while..."
cd ../gcc-14.1.0
../../src/gcc-14.1.0/configure  \
	--target="$TARGET"			\
	--prefix="$PREFIX"			\
	--disable-nls				\
	--enable-languages=c		\
	--without-headers

make all-gcc all-target-libgcc -j $(nproc)
make install-strip-gcc install-strip-target-libgcc
	
echo "Cleaning up"
rm -rf toolchain/build
rm -rf toolchain/src
