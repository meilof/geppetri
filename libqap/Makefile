LIBQAP_OBJ = base.o fft.o key.o modp.o proof.o prove.o qap.o qap2key.o verify.o

all: libqap.a

libqap.a:  $(LIBQAP_OBJ)
	ar rcs libqap.a $(LIBQAP_OBJ)

%.o: %.cpp
	g++ -std=c++11 -I ../ate-pairing -I ../ate-pairing/include -c -o $@ $<
