# how to build all the dependencies
this shoudn't be seen as a tutorial, but rather as a reminder to myself about how to do this again

keep in mind that these steps might stop working at anytime devkitpro updates

## enable windows symlink support
at default symlinks just fail to get created by ln -s, this makes them work
edit msys2_shell.bat at devkitpro/msys2 directory
edit the line
```
rem set MSYS=winsymlinks:nativestrict
```
into
```
set MSYS=winsymlinks:nativestrict
```
save, and start msys2_shell.bat as admin
navigate to the third-party directory and build all the libs

# libjpeg-turbo 1.5.3
```
$ PATH=$DEVKITARM/bin:$PATH sh configure --prefix=$DEVKITPRO/portlibs/arm --host=arm-none-eabi --disable-shared --enable-static
$ PATH=$DEVKITARM/bin:$PATH make install
```
# libogg 1.3.4
```
$ PATH=$DEVKITARM/bin:$PATH sh configure --prefix=$DEVKITPRO/portlibs/arm --host=arm-none-eabi --disable-shared --enable-static
$ PATH=$DEVKITARM/bin:$PATH make install
```
# libtremor lowmem branch
```
$ PATH=$DEVKITARM/bin:$PATH sh autogen.sh --prefix=$DEVKITPRO/portlibs/arm --host=arm-none-eabi --disable-shared --enable-static
$ PATH=$DEVKITARM/bin:$PATH make install
```
# zlib 1.2.11
```
$ pacman -S binutils
$ PATH=$DEVKITARM/bin:$PATH prefix=$DEVKITPRO/portlibs/arm CC=arm-none-eabi-gcc ./configure --static
$ PATH=$DEVKITARM/bin:$PATH make install
```
# libpng 1.6.37
```
$ PATH=$DEVKITARM/bin:$PATH sh configure --prefix=$DEVKITPRO/portlibs/arm --host=arm-none-eabi --disable-shared --enable-static CPPFLAGS=-I$DEVKITPRO/portlibs/arm/include LDFLAGS=-L$DEVKITPRO/portlibs/arm/lib
$ PATH=$DEVKITARM/bin:$PATH make install
```
# libmad 0.15.1b
windows msys2 only: comment out this line from the Makefile.in by prepending a #
```
#  cd $(srcdir) && $(ACLOCAL) $(ACLOCAL_AMFLAGS)
```
--build=mingw32 flag is only necessary on windows msys2
```
$ PATH=$DEVKITARM/bin:$PATH sh configure --prefix=$DEVKITPRO/portlibs/arm --host=arm-none-eabi --disable-shared --enable-static --build=mingw32
```
remove the flag -fforce-mem from CFLAGS in the Makefile, then
```
$ PATH=$DEVKITARM/bin:$PATH make install
```
the Makefile might get rebuilt after the make install and fail because of the --force-mem flag, in that case just remove the flag again and run the last make install command again

# done
if everything went right all the libs got installed at $DEVKITARM/portlibs/arm
to use them in your homebrew, link the libraries at LIBS :=
and add the path $(DEVKITPRO)/portlibs/arm to LIBDIRS :=
check out the Makefile for dds.nds for an example