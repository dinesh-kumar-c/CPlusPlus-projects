#pragma once

#include <map>
#include <cmath>
#include <unordered_map>


struct Order
{
	int order_id;
	char side;
	int size;
	uint64_t price;
};

//Key for these maps = price*std::pow(10,8), this will keep
//comparisons consistent in the map
typedef std::map<uint64_t,Order,std::greater<uint64_t>> buySide_t;
typedef std::map<uint64_t,Order> sellSide_t;
typedef std::unordered_map<int,std::map<uint64_t,Order>::iterator> OrderIdMap_t;
typedef std::unordered_map<int,Order> OrderIdOrderMap_t;

class OrderBook
{
  private:
	sellSide_t sellSide;
	buySide_t buySide;
	Order orderDetails;
	OrderIdOrderMap_t orderIdOrderMap;
	OrderIdMap_t orderIdMap;

    public :

    OrderBook();
    ~OrderBook();
    void add(int order_id, char side, double price, int size);
    void modify(int order_id, int new_size);
    void remove(int order_id);
    double get_price(char side, int level);
    int get_size(char side, int level);
};

