
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
# ======================================================
.PHONY: all 


# Use color enabled GCC output for readability
# ======================================================
#CC       = colorgcc # C Compiler
 CC       = clang    # C Compiler - improved diagnosability


# ======================================================
 CFLAGS  += -c 
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
# "all" should always be the first defined target to ensure default build
all: prepare \
     build   \
     cleanup

prepare: 
	@echo "" 
	@echo "Prepare:" 
	@echo "========"
	rm -fr bin
build:
	@echo ""
	@echo "Compile Shared Library:"
	@echo "======================="
	$(CC) -I ${PWD} -fPIC $(CFLAGS) $(C_FILES) 
	$(CC) -I ${PWD} -shared -o libadts.so $(OBJECTS) -lrt

cleanup:
	@echo ""
	@echo "Cleanup:"
	@echo "========"
	mkdir -p bin
	mv *.[i,s,o] bin/
	mv *.so bin/


