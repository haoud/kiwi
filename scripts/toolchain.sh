#!/bin/bash
set -e

# Parse arguments
while [ "$1" != "" ]; do
	case $1 in
		# If the user wants to force the installation, remove the toolchain
		# directory to allow the script to reinstall the toolchain
		--force ) rm -rf toolchain
			;;
	esac
	shift
done

# Check if the toolchain is already installed. In this case, we don't want to
# reinstall it to avoid overwriting the user's toolchain
if [ -d toolchain ]; then
	echo "Toolchain already installed" 1>&2;
	echo "If you want to reinstall it, remove the toolchain directory or use\
	      the --force flags" 1>&2;
	exit 1;
fi

echo "Creating required directories"
mkdir -p bin
mkdir -p toolchain/src
mkdir -p toolchain/build/binutils-2.42
mkdir -p toolchain/build/gcc-14.1.0
	
# Downloading and extracting binutils if not already done
if [ ! -f toolchain/src/binutils-2.42.tar.xz ]; then
	echo "Downloading binutils-2.42.tar.gz"
	wget https://mirror.ibcp.fr/pub/gnu/binutils/binutils-2.42.tar.xz \
		-O toolchain/src/binutils-2.42.tar.xz
fi

# Downloading and extracting gcc if not already done
if [ ! -f toolchain/src/gcc-14.1.0.tar.xz ]; then
	echo "Downloading gcc-14.1.0.tar.gz"	
	wget https://mirror.ibcp.fr/pub/gnu/gcc/gcc-14.1.0/gcc-14.1.0.tar.xz \
		-O toolchain/src/gcc-14.1.0.tar.xz
fi
	
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
