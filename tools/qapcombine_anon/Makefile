include ../../commons.mk

qapcombine_anon: main.o
	g++ -o $@ $< $(LDFLAGS)

%.o: %.cpp
	g++ $(CFLAGS) -c -o $@ $<
