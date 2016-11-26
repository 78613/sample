Instructions
============
Setup the library path here for auto execution from samples/Makefile

http://serverfault.com/questions/201709/how-to-set-ld-library-path-in-ubuntu

See sample/src/utest/Makefile
- code hardcodes the searchpath for libadts.so shared library.  
- This can be enhanced as needed, but no immediate use case at this time.


Example
=======
export LD_LIBRARY_PATH="/home/ocardona/Documents/C/src/adts/bin/":$LD_LIBRARY_PATH


To diff the code use:
====================
# git difftool
or
# meld
