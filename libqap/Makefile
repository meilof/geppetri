LIBQAP_OBJ = base.o fft.o key.o modp.o proof.o prove.o qap.o qap2key.o verify.o

include ../commons.mk

all: libqap.a

libqap.a:  $(LIBQAP_OBJ)
	ar rcs libqap.a $(LIBQAP_OBJ)

%.o: %.cpp
	g++ $(CFLAGS_LIB) -c -o $@ $<
