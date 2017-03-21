qapver_stat: vermain.o
	g++ -o $@ $< -L ../../libqap -L ../../ate-pairing/lib -lqap -lm -lzm -lgmpxx -lgmp

%.o: %.cpp
	g++ -std=c++11 -I ../.. -I ../../ate-pairing/include -c -o $@ $<
