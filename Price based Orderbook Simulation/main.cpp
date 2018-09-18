#include <iostream>
#include "orderbook.h"

//const char *inputFile = "test.in";

int main(int argc, char **argv)
{

/*    Parser myParser(currentDate, "myTestFile");

    int fd = open(inputFile, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Couldn't open %s\n", inputFile);
        return 1;
    }



    close(fd);*/
	//###TestCase 1###
	OrderBook book;
	book.add(1,'B',45.2,100);
	book.modify(1,50);
	double price = book.get_price('B',1);
	std::cout<<price<<"\n";
	book.add( 2,'S',51.4,200);
	book.add(3,'B',45.1,100);
	int size = book.get_size ('S',1);
	std::cout<<size<<"\n";
	book.add(4,'S',51.2,300);
	book.add(5,'S',51.2,200);
	//book.add(6,'S',51,200);
	book.remove(3);
	price=book.get_price ('B',1);
	std::cout<<price<<"\n";
	size = book.get_size('B',1);
	std::cout<<size<<"\n";
	price=book.get_price( 'S',1);
	std::cout<<price<<"\n";
	//size = book.get_size('S',1);
	size = book.get_size('S',1);
	std::cout<<size<<"\n\n\n";


	//test case #2
	OrderBook book2;
	book2.add(1, 'B', 22.5, 100);
	book2.add( 2 ,'S', 37.8, 250);
	book2.add( 3 ,'B', 24.7, 150);
	price = book2.get_price( 'B', 1); //this returns 24.7
	std::cout<<price<<"\n";
	price = book2.get_price( 'B', 2 );//this returns 22.5
	std::cout<<price<<"\n";
	book2.modify( 3, 50);
	book2.add( 4, 'S', 35.1, 250);
	book2.add( 5, 'S', 37.8, 150);
	price=book2.get_price( 'S', 1);//this returns 35.1
	std::cout<<price<<"\n";
	book2.remove( 3);
	size=book2.get_size( 'S', 1 );//this returns 250
	std::cout<<size<<"\n";
	size=book2.get_size( 'S' ,2 );//this returns 400.
	std::cout<<size<<"\n";
	book2.remove( 5);
	book2.add( 6, 'S',37.8, 150);
	book2.add( 7, 'S', 37.6, 350);
	book2.add( 8, 'B', 24.7, 200);
	size=book2.get_size( 'B', 1 );//this returns 200
	std::cout<<size<<"\n";
	price=book2.get_price( 'S', 2);//this returns 37.6
	std::cout<<price<<"\n";
	book2.modify( 8 ,150);
	book2.add( 9 ,'S', 35.1, 200);
	book2.add( 10, 'B', 22.5, 350);
	size=book2.get_size( 'B', 2 );//this returns 450
	std::cout<<size<<"\n";
	price=book2.get_price( 'S', 3);//this returns 37.8
	std::cout<<price<<"\n";

    return 0;
}
