#!/bin/bash

cc -Wall -c -g  mseedconvert.c
cc mseedconvert.o /home/jeff/libmseed/libmseed.a

#cc -Wall -O2  /home/jeff/xseed-utils/libsrc/libmseed/src/MSEED_LIBRARY-build/libmseed.a -I/home/jeff/xseed-utils/libsrc/libmseed/src/MSEED_LIBRARY-build/  mseedconvert.c -o mseedconvert
