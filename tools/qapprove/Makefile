include ../../commons.mk

qapprove: provemain.o
	g++ -o $@ $< $(LDFLAGS)

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<
