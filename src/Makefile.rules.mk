
# ======================================================
#xCC      = colorgcc # C Compiler - color enabled
 xCC      = clang    # C Compiler - improved diagnosability


# search online for CLANG/GCC flags referrence
# ======================================================
 
# Debug information:
# ======================================================
 xCFLAGS  += --debug # Enable debugging information

 xCFLAGS  += -fverbose-asm
 xCFLAGS  += -w # hide all warnings 
 xCFLAGS  += -save-temps # Generate preprocessor files - *.i
#xCFLAGS  += -std=gnu99 # Use the c99 standard / with gnu extensions
#xCFLAGS  += -std=c11
 xCFLAGS  += -Wall # All warnings enabled
 xCFLAGS  += -Wextra
 xCFLAGS  += -O3 # Optimization level

#xCFLAGS  += -v # Display compilation commands / stages
 xCFLAGS  += -Q # Print compiled function and statistics
#xCFLAGS  += -Werror 
#xCFLAGS  += -pedantic # Warnings demanded by strict ISO C


# Local project flag definitions
# ======================================================
 xCFLAGS  += -D__ADTS_DISPLAY # Display output to console

