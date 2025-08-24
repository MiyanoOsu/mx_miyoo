
# MX

A SDL legacy frontend for miyoo cfw which i created from scratch

Target v90 q90 q20

# How to use

Copy all forder in source code to same folder of binary to run, also copy all sections in gmenu2x folder to sections folder to add all your sections from gmenu2x

if you want to boot to MX when open handheld, just copy file autoexec.sh to folder MAIN. Make sure path in file matches with binary, and autoexec.sh have exec permissions

# How to build
You need docker, debian linux or arch linux
~~~
# if you first time build code, run this command to set up toolchain
docker pull miyoocfw/toolchain-shared-uclibc:latest

# to compile
docker run --volume ./:/src/ -it miyoocfw/toolchain-shared-uclibc:latest
cd src
make -f makefile.miyoo -j $(nproc)
~~~
