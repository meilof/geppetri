include ../../commons.mk

qapgen: genmain.o
	g++ -o $@ $< $(LDFLAGS)

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<
