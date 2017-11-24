# ======================================================
# Makefile
#    |
#    +----- Makefile.files // Source Code
#    +----- Makefile.rules // Common compulation rules
#
# ======================================================


# Files specified separetely.
# ======================================================
include Makefile.files.mk
H_FILES += $(xH_FILES)
C_FILES += $(xC_FILES)


# Rules are common to all compilation units.	
# ======================================================
include ../Makefile.rules.mk
CC      = $(xCC)
CFLAGS += $(xCFLAGS)


# ======================================================
LPATHS += -L../adts/bin
LNAMES += -ladts

HPATHS += -I .
HPATHS += -I ../adts/


# Everything below this line is standard Makefile logic:
# ======================================================
DEPS    += $(H_FILES)
OBJECTS += $(patsubst %.c,%.o,$(C_FILES))    


# declare non-files
# ======================================================
.PHONY: all 


# "all" should always be the first defined target to ensure default build
# ======================================================
all: clean  \
     compile  \
     cleanup  \
     unittest

clean:
	@echo ""
	@echo "Clean:"
	@echo "======"
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
	mv *.bc bin/

unittest:
	@echo ""
	@echo "Unit Test:"
	@echo "=========="
	bin/utest.exe 


# ======================================================
# If compilation failure, see README.md file in ths git repository.
# ======================================================
