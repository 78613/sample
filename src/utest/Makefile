
# ======================================================
HFILES  += pe.h
HFILES  += cci.h
HFILES  += epi.h
HFILES  += misc.h
HFILES  += utest.h
HFILES  += coursera.h

CFILES  += pe.c
CFILES  += cci.c
CFILES  += epi.c
CFILES  += misc.c
CFILES  += utest.c
CFILES  += coursera.c


# Everything below this line is standard Makefile logic:
# ======================================================
DEPS    += $(HFILES)
OBJECTS += $(patsubst %.c,%.o,$(CFILES))    

# declare non-files
.PHONY: all 

# ======================================================
# Use color enabled GCC output for readability
 CC       = colorgcc

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


# ======================================================
# "all" should always be the first defined target to ensure default build
all: unittest.exe

unittest.exe: 
	@echo ""
	@echo "Cleanup:"
	@echo "========"
	rm -fr bin

	@echo ""
	@echo "Compile:"
	@echo "========"
	mkdir -p bin
	$(CC) $(HPATHS) $(CFLAGS) -o bin/utest.exe $(CFILES) $(LPATHS) $(LNAMES)
	mv *.[i,s,o] bin/

	@echo ""
	@echo "Unit Test:"
	@echo "=========="
   #LD_LIBRARY_PATH=../adts/bin/:$(LD_LIBRARY_PATH)
	bin/utest.exe 


