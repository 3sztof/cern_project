.PHONY: all clean
vpath %.h ../include
CXXFLAGS=-Wall -O -I../include
all: make_ref
make_ref: make_ref.C ../include/fmcRef.h
	$(LINK.cc) $(filter-out %.h, $^) $(LOADLIBES) $(LDLIBS) -o $@
clean:
	rm -f make_ref
