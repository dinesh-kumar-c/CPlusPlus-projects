OBJS = Parser.o

all: orderbook

orderbook: main.cpp orderbook.cpp
	g++ -W -O3 -std=c++14 -o $@ $^

%.o : %.cc
	g++ -W -O3 -c -std=c++14 -o $@ $<

#libparser.a: $(OBJS)
#	ar rcs libparser.a $^

clean:
	rm -f *.o *.a feed
