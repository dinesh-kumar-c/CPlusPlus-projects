OBJS = Parser.o

all: feed

feed: main.cc messages.cc libparser.a
	g++ -W -O3 -std=c++17 -o $@ $^

%.o : %.cc
	g++ -W -O3 -c -std=c++17 -o $@ $<

libparser.a: $(OBJS)
	ar rcs libparser.a $^

clean:
	rm -f *.o *.a feed
