#!/bin/bash
echo "x64 dependencies builder 1.0.0"

zlibArchive='https://github.com/madler/zlib/releases/download/v1.2.13/zlib-1.2.13.tar.xz'
caresArchive='https://github.com/c-ares/c-ares/releases/download/cares-1_19_0/c-ares-1.19.0.tar.gz'
wolfSSLArchive='https://github.com/wolfSSL/wolfssl/archive/refs/tags/v5.6.0-stable.tar.gz'
nghttp2Archive='https://github.com/nghttp2/nghttp2/releases/download/v1.52.0/nghttp2-1.52.0.tar.xz'
curlArchive='https://github.com/curl/curl/releases/download/curl-8_0_1/curl-8.0.1.tar.xz'

echo -n "Install tools... "
sudo apt update && sudo apt install tar xz-utils curl cmake make autoconf libtool -y

echo -n "Get latest musl toolchain... "
mkdir dependencies
cd dependencies
curl -s -L https://musl.cc/armv7l-linux-musleabihf-cross.tgz | tar zx

echo -n "Set toolchain vars... "
export TOOLCHAIN=$(pwd)/armv7l-linux-musleabihf-cross
export AR=$TOOLCHAIN/bin/armv7l-linux-musleabihf-gcc-ar
export AS=$TOOLCHAIN/bin/armv7l-linux-musleabihf-as
export CC=$TOOLCHAIN/bin/armv7l-linux-musleabihf-gcc
export CXX=$TOOLCHAIN/bin/armv7l-linux-musleabihf-g++
export LD=$TOOLCHAIN/bin/armv7l-linux-musleabihf-ld
export RANLIB=$TOOLCHAIN/bin/armv7l-linux-musleabihf-gcc-ranlib
export STRIP=$TOOLCHAIN/bin/armv7l-linux-musleabihf-strip


#zlib
echo -n "Download and Extract zlib... "
curl -s -L $zlibArchive | tar --xz -x

echo -n "Configure zlib... "
cd zlib*
./configure --host=armv7l-linux-musleabihf --prefix=$TOOLCHAIN --static

echo -n "Build zlib... "
make -j$(nproc)

echo -n "Install zlib... "
make install 
cd ..


#c-ares
echo -n "Download and Extract c-ares... "
curl -s -L $caresArchive | tar zx

echo -n "Configure c-ares... "
cd c-ares*
./configure --prefix=$TOOLCHAIN --host=armv7l-linux-musleabihf --disable-shared 

echo -n "Build c-ares... "
make -j"$(nproc)" 

echo -n "Install c-ares... "
make install 
cd ..


#wolfSSL
echo -n "Download and Extract wolfSSL... "
curl -s -L $wolfSSLArchive | tar xz

echo -n "Configure wolfSSL... "
cd wolfssl*
./autogen.sh 
./configure --host=armv7l-linux-musleabihf --enable-curl --prefix=$TOOLCHAIN --enable-static --disable-shared --enable-all-crypto --with-libz=$TOOLCHAIN 

echo -n "Build wolfSSL... "
make

echo -n "Install wolfSSL... "
make install 
cd ..


#nghttp2
echo -n "Download and Extract nghttp2... "
curl -s -L $nghttp2Archive | tar --xz -x

echo -n "Configure nghttp2... "
cd nghttp2*
./configure --host=armv7l-linux-musleabihf --enable-lib-only --disable-shared --prefix=$TOOLCHAIN

echo -n "Build nghttp2... "
make -j"$(nproc)" 

echo -n "Install nghttp2... "
make install
cd ..


#cURL
echo -n "Download and Extract curl... "
curl -s -L $curlArchive | tar --xz -x

echo -n "Configure curl... "
cd curl*
./configure --host=armv7l-linux-musleabihf --disable-shared --prefix=$TOOLCHAIN --with-wolfssl=$TOOLCHAIN --enable-ares=$TOOLCHAIN --with-nghttp2=$TOOLCHAIN 

echo -n "Build cURL... "
make -j"$(nproc)" 

echo -n "Install cURL... "
make install 

echo -e "\e[32mAll done\e[0m"


exit 0