
# Files specified separetely.
# ======================================================
include Makefile.files
H_FILES += $(xH_FILES)
C_FILES += $(xC_FILES)


# Everything below this line is standard Makefile logic:
# ======================================================
DEPS    += $(H_FILES)
OBJECTS += $(patsubst %.c,%.o,$(C_FILES))    

# declare non-files
.PHONY: all 

# ======================================================
# Use color enabled GCC output for readability
#CC       = colorgcc
 CC       = clang

 CFLAGS  += -g # Produce debugging information
 CFLAGS  += -fverbose-asm
 CFLAGS  += -w # hide all warnings 
 CFLAGS  += -save-temps
 CFLAGS  += -std=gnu99 # Use the c99 standard / with gnu extensions
 CFLAGS  += -Wall 
 CFLAGS  += -Wextra
 CFLAGS  += -O3
 CFLAGS  += -D__ADTS_DISPLAY

#CFLAGS  += -v 
#CFLAGS  += -Q # Print compiled function and statistics
#CFLAGS  += -Werror 
#CFLAGS  += -pedantic


# ======================================================
 LPATHS += -L../adts/bin
 LNAMES += -ladts

 HPATHS += -I .
 HPATHS += -I ../adts/


# ======================================================
# "all" should always be the first defined target to ensure default build
all: prepare  \
     compile  \
     cleanup  \
     unittest

prepare:
	@echo ""
	@echo "Prepare:"
	@echo "========"
	rm -fr bin
compile:
	@echo ""
	@echo "Compile:"
	@echo "========"
	mkdir -p bin
	$(CC) $(HPATHS) $(CFLAGS) -o bin/utest.exe $(C_FILES) $(LPATHS) $(LNAMES)
cleanup:
	@echo ""
	@echo "Cleanup:"
	@echo "========"
	mv *.[i,s,o] bin/
unittest:
	@echo ""
	@echo "Unit Test:"
	@echo "=========="
	bin/utest.exe 


# command to export the *.so for compilation:
# -------------------------------------------
#export LD_LIBRARY_PATH="/home/parallels/Documents/github/sample/sample/src/adts/bin/":$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH="/home/ocardona/Documents/sample/src/adts/bin/":$LD_LIBRARY_PATH

# Step by step:
# http://serverfault.com/questions/201709/how-to-set-ld-library-path-in-ubuntu
