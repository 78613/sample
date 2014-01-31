
# ======================================================
SUBDIRS += adts
SUBDIRS += utest


# ======================================================
all: control

# ordered subdirectory walking
control:
	-for d in $(SUBDIRS); do (cd $$d; $(MAKE)); done

