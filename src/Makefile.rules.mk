
# ======================================================
#xCC      = colorgcc # C Compiler - color enabled
 xCC      = clang    # C Compiler - improved diagnosability


# search online for CLANG/GCC flags referrence
# ======================================================
 
# Debug information:
# ======================================================
 xCFLAGS  += --debug # Enable debugging information


 xCFLAGS  += -fverbose-asm # assembly output
 xCFLAGS  += -save-temps # Generate preprocessor files - *.i


 xCFLAGS  += -std=gnu99 # Use the c99 standard / with gnu extensions
#xCFLAGS  += -std=c99


 # Enable all warnings:
 xCFLAGS  += -w           # hide all warnings - overrides all settings below
 xCFLAGS  += -Weverything # enable all CLANG warnings
 xCFLAGS  += -Wall        # All warnings enabled
 xCFLAGS  += -Wextra
 xCFLAGS  += -Werror 
 xCFLAGS  += -pedantic    # Warnings demanded by strict ISO C


 # Disable specific warnings:
 xCFLAGS  += -Wno-gnu-zero-variadic-macro-arguments
 xCFLAGS  += -Wno-missing-prototypes

#xCFLAGS  += -ftrapv # abort on signed integer overflow


#xCFLAGS  += -v # Display compilation commands / stages
#xCFLAGS  += -Q # Print compiled function and statistics


 xCFLAGS  += -Ofast # Enables all the optimizations from -O3 along with other 
                    # aggressive optimizations that may violate strict 
                    # compliance with language standards



# Local project flag definitions
# ======================================================
 xCFLAGS  += -D__ADTS_DISPLAY # Display output to console

