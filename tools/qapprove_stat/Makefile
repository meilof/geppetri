include ../../commons.mk

qapprove_stat: provemain.o
	g++ -o $@ $< $(LDFLAGS)

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<
