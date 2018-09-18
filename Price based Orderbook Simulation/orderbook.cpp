#include <string>
#include <iostream>
#include <iterator>
#include "orderbook.h"

OrderBook::OrderBook()
{
}

OrderBook::~OrderBook()
{
}


void OrderBook::add(int order_id, char side, double price, int size)
{
	try
	{
		orderDetails.order_id=order_id;
		orderDetails.side=side;
		orderDetails.price =price*std::pow(10,8);
		orderDetails.size=size;
		orderIdOrderMap.emplace(std::make_pair(order_id,orderDetails));

		if (side == 'S')
		{
			auto result=sellSide.emplace(std::make_pair(orderDetails.price,orderDetails));
			if (result.second)
			{
				orderIdMap[order_id]=result.first;
			}
			else
			{
				result.first->second.size+=size;
			}
		}
		else
		{
			auto result=buySide.emplace(std::make_pair(orderDetails.price,orderDetails));
			if (result.second)
			{
				orderIdMap[order_id]=result.first;
			}
			else
			{
				result.first->second.size+=size;
			}
		}

	}
	catch (...)
	{
		//relevent catches can be added
		std::cerr<<"Error Processing onAddOrder mesasage!!!!!\n";
	}
}

void OrderBook::modify(int order_id, int new_size)
{
	try
	{
		auto found = orderIdMap.find(order_id);
		auto foundOrderIdOrderMap = orderIdOrderMap.find(order_id);
		int oldSize=0;

		if ( foundOrderIdOrderMap == orderIdOrderMap.end() )
		{
/*			std::cout<<"Order id="<<order_id<<"\n";
			std::cerr<<"remove::!!!!Order not found in orderIdOrderMap!!!!!!\n";*/
		}
		else
		{
			oldSize=(foundOrderIdOrderMap->second).size;
			(foundOrderIdOrderMap->second).size=new_size;
		}

		if ( found == orderIdMap.end() )
		{
			//std::cerr<<"Modify::!!!!Order not found in orderIdMap!!!!!!\n";
		}
		else
		{
			auto sideMapit = found->second;
			(sideMapit->second).size=(sideMapit->second).size-oldSize+new_size;
		}
	}
	catch (...)
	{//relevent catches can be added
		std::cerr<<"Error Processing modifyOrder mesasage!!!!!\n";
	}
}


void OrderBook::remove(int order_id)
{
	try
	{
		auto foundOrderIdMap = orderIdMap.find(order_id);
		auto foundOrderIdOrderMap = orderIdOrderMap.find(order_id);
		int delSize=0;

		if ( foundOrderIdOrderMap == orderIdOrderMap.end() )
		{
			//std::cout<<"Order id="<<order_id<<"\n";
			std::cerr<<"remove::!!!!Order not found in orderIdOrderMap!!!!!!\n";
		}
		else
		{
			delSize=(foundOrderIdOrderMap->second).size;
			//std::cout<<"delsize:"<<delSize<<"\n";
			orderIdOrderMap.erase(foundOrderIdOrderMap);
		}


		/*for (auto &it : orderIdOrderMap)
			std::cout<< "OrderId="<<it.first<<"\n";

		for (auto &it2 : orderIdMap)
			std::cout<< "OrderId="<<it2.first<<"\n";*/

		if ( foundOrderIdMap == orderIdMap.end() )
		{
			//std::cout<<"Order id="<<order_id<<"\n";
			//std::cerr<<"remove::!!!!Order not found in orderIdMap!!!!!!\n";
		}
		else
		{
			auto sideMapit = foundOrderIdMap->second;
			//std::cout<<"sizeInSideMap="<<(sideMapit->second).size<<"\n";
			(sideMapit->second).size -=delSize;
			if ((sideMapit->second).side == 'B')
			{
				if ((sideMapit->second).size ==0)
				{
					buySide.erase(sideMapit);
					orderIdMap.erase(foundOrderIdMap);
				}
			}
			else
			{
				//the order is in SELL side map
				if ((sideMapit->second).size ==0)
				{
					sellSide.erase(sideMapit);
					orderIdMap.erase(foundOrderIdMap);
				}
			}
		}
	}
	catch (...)
	{//relevent catches can be added
		std::cerr<<"Error Processing CancelledOrder mesasage!!!!!\n";
	}
}

double OrderBook::get_price(char side, int level)
{
	try
	{
		if (side == 'B')
		{
			if (!buySide.empty())
			{
				auto found= buySide.begin();
				if (level >1)
					std::advance(found,level-1);
				return (found->second.price)/std::pow(10,8);
			}
			else
				throw std::string("No Buy orders yet!!\n");

		}
		else
		{
			if (!sellSide.empty())
			{
				auto found= sellSide.begin();
				if (level >1)
					std::advance(found,level-1);
				return (found->second.price)/std::pow(10,8);
			}
			else
				throw std::string("No Sell orders yet!!\n");
		}

	}
	catch (std::string error)
	{
		std::cerr<<error;
	}
	catch (...)
	{//relevent catches can be added
		std::cerr<<"Error Processing CancelledOrder mesasage!!!!!\n";
	}
}

int OrderBook::get_size(char side, int level)
{
	try
	{
		if (side == 'B')
		{
			if (!buySide.empty())
			{
				auto found= buySide.begin();
				if (level >1)
					std::advance(found,level-1);
				return (found->second.size);
			}
			else
				throw std::string("No Buy orders yet!!\n");

		}
		else
		{
			if (!sellSide.empty())
			{
/*				for (auto it=sellSide.begin();it != sellSide.end();it++)
					std::cout<<it->second.price<<" ;"<<it->second.size<<"\n";*/
				auto found= sellSide.begin();
				if (level >1)
					std::advance(found,level-1);
				return (found->second.size);
			}
			else
				throw std::string("No Sell orders yet!!\n");
		}

	}
	catch (std::string error)
	{
		std::cerr<<error;
	}
	catch (...)
	{//relevent catches can be added
		std::cerr<<"Error Processing CancelledOrder mesasage!!!!!\n";
	}
}



