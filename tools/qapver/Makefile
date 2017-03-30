include ../../commons.mk

qapver: vermain.o
	g++ -o $@ $< $(LDFLAGS)

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<
