

HFILES  += adts_bits.h
HFILES  += adts_time.h
HFILES  += adts_hexdump.h
HFILES  += adts_snapshot.h
HFILES  += adts_services.h

CFILES  += adts_bits.c
CFILES  += adts_time.c
CFILES  += adts_hexdump.c
CFILES  += adts_snapshot.c

# Everything below this line is standard Makefile logic:
# ======================================================
DEPS    += $(HFILES)
OBJECTS += $(patsubst %.c,%.o,$(CFILES))    

# declare non-files
.PHONY: all 

# Use color enabled GCC output for readability
 CC       = colorgcc

 CFLAGS  += -c 
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
 CFLAGS  += -D__SERVICES_DISPLAY


# "all" should always be the first defined target to ensure default build
all: output

output: 
	@echo "" 
	@echo "Cleanup:" 
	@echo "========"
	rm -fr bin

	@echo ""
	@echo "Compile Shared Library:"
	@echo "======================="
	mkdir -p bin
	$(CC) -I ${PWD} -fPIC $(CFLAGS) $(CFILES) 
	$(CC) -I ${PWD} -shared -o libadts.so $(OBJECTS) -lrt
	mv *.[i,s,o] bin/
	mv *.so bin/

#end of file