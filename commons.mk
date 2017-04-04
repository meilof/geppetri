# cflags for libqap
CFLAGS_LIB=-std=c++11 -I ../ate-pairing -I ../ate-pairing/include -I /usr/local/include

# cflags/ldflags for tools
CFLAGS=-std=c++11 -I ../.. -I ../../ate-pairing/include -I /usr/local/include
LDFLAGS=-L ../../libqap -L ../../ate-pairing/lib -L/usr/local/lib -lqap -lm -lzm -lgmpxx -lgmp
