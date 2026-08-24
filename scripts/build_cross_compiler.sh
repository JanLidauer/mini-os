#!/bin/bash
set -e  # safety measure, cancels script if error occurs


# export marks an enviroment variable
export PREFIX="/root/opt/cross" #location where cross compiler should be
export TARGET=i686-elf          # target architecture i686-elf ="32-Bit x86
export PATH="$PREFIX/bin:$PATH"  #adds the path we created to the linux path variable so the programm can be found

mkdir -p /root/src #creates src dir
cd /root/src # goes to src dir


#sets fixed versions for binary utils and gcc so there are no version mismatches
BINUTILS_VERSION="2.42"
GCC_VERSION="13.2.0"

#if statement checks if an tar with the binutils of the set version already exists or else downloads it
if [ ! -f "binutils-$BINUTILS_VERSION.tar.gz" ]; then
    wget "https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz"
fi

tar -xzf "binutils-$BINUTILS_VERSION.tar.gz" # unextract the tar file

#makes a new dir for the build process and switches to it
mkdir -p build-binutils
cd build-binutils

#goes into the folder we unpacked and uses the configure script inside, we give our architecture and our location to the script, configure creates a make file
../binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror 

#read the makefile and compileit
make -j$(nproc)
#calls the install in the makefile and saves the programms to the place we specified with prefix
make install
cd ..

#now we basically do the same for the cross compiler that we did for binutils
if [ ! -f "gcc-$GCC_VERSION.tar.gz" ]; then
    wget "https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz"
fi
tar -xzf "gcc-$GCC_VERSION.tar.gz"

mkdir -p build-gcc
cd build-gcc
../gcc-$GCC_VERSION/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers #--enable-languages=c = we only need c, saves time --without-headers we have no default librarys
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc

echo "Cross-Compiler build was succesfull: $PREFIX/bin/$TARGET-gcc"