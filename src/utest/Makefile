
CFILES  += utest.c

# Everything below this line is standard Makefile logic:
# ======================================================
DEPS    += $(HFILES)
OBJECTS += $(patsubst %.c,%.o,$(CFILES))    

# declare non-files
.PHONY: all 

# Use color enabled GCC output for readability
 CC       = colorgcc

#CFLAGS  += -c 
 CFLAGS  += -g # Produce debugging information
#CFLAGS  += -Q # Print compiled function and statistics
 CFLAGS  += -fverbose-asm
#CFLAGS  += -v 
 CFLAGS  += -w # hide all warnings 
 CFLAGS  += -save-temps
 CFLAGS  += -std=gnu99 # Use the c99 standard / with gnu extensions
 CFLAGS  += -Wall 
 CFLAGS  += -Wextra
#CFLAGS  += -Werror 
#CFLAGS  += -pedantic
 CFLAGS  += -O3
 CFLAGS  += -pthread #pthread_yield()

 LPATHS += -L../adts/bin

#LNAMES += -lpthread
#LNAMES += -lrt
 LNAMES += -ladts

#HPATHS += ${PWD}
 HPATHS += -I ../adts/

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
    #$(CC) -I ${PWD} $(CFLAGS) -o bin/utest.exe $(CFILES) -lpthread -lrt -ladts
    #gcc -static main.c -L. -lmean -o statically_linked
	mv *.[i,s,o] bin/

	@echo ""
	@echo "Unit Test:"
	@echo "=========="
	LD_LIBRARY_PATH=../adts/bin/:$(LD_LIBRARY_PATH)
	bin/utest.exe 

#end of file

