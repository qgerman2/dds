# dds - a stepmania / ddr clone for the nintendo ds
this project aims to recreate a game experience with **minimal compromises** such as not requiring a specific sound file format or specific image dimentions, every song should work drag and drop just like the other games that support them, the only limitations being video backgrounds or lua scripts.

# building
you should compile this project with the latest devkitpro release from their website, either use the windows installer or the pacman repository


check out the third-party directory for building the required third-party libraries not offered by devkitpro, which are
* libmad : MPEG Audio Decoder
* libogg and libremor: Integer-only Ogg vorbis decoder
* libjpeg-turbo : jpeg image decoder
* zlib : data compression, dependency for libpng
* libpng : png image decoder


with everything set up, make should generate a working .nds file
