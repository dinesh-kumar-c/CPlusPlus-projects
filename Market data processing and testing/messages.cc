#include "messages.h"
#include <iostream>

using namespace messages;

Order AddOrderInMsg::store()
{
	orderDetails.orderRefNum=this->getOrderRefNum();
	orderDetails.timeStamp =this->getTimeStamp();
	orderDetails.side=this->getSide();
	orderDetails.size =this->getSize();
	memset(orderDetails.stockTicker,'\0',sizeof(orderDetails.stockTicker));
	memcpy(orderDetails.stockTicker,this->getStockTicker().c_str(),sizeof(orderDetails.stockTicker));
	orderDetails.price=this->getPrice();
	return orderDetails;
}

void AddOrderOutMsg::copy(AddOrderInMsg& cpyMsg)
{
	memset(addOut.stockTicker,'\0',sizeof(addOut.stockTicker));
	memcpy(addOut.stockTicker,cpyMsg.getStockTicker().c_str(),sizeof(addOut.stockTicker));
	addOut.orderRefNum=cpyMsg.getOrderRefNum();
	addOut.side=cpyMsg.getSide();
	addOut.size=cpyMsg.getSize();
	addOut.price=cpyMsg.getPrice();
	addOut.timeStamp=cpyMsg.getTimeStamp();
}

void AddOrderOutMsg::dump()
{
	std::cout<<"messageType="<<addOut.msgType<<"\n";
	std::cout<<"messageSize="<<addOut.msgSize<<"\n";
	std::cout<<"Ticker="<<this->getStockTicker()<<"\n";
	std::cout<<"timeStamp="<<this->getTimeStamp()<<"\n";
	std::cout<<"OrderRefNum="<<this->getOrderRefNum()<<"\n";
	std::cout<<"Side="<<this->getSide()<<"\n";
	std::cout<<"Size="<<this->getSize()<<"\n";
	std::cout<<"Price="<<this->getPrice()<<"\n";
}

void ExecutedOrderInMsg::store(Order& order)
{
	order.timeStamp =this->getTimeStamp();
	order.filled = order.filled + this->getSize();
}

void ExecutedOrderOutMsg::copy(Order& order)
{
	memset(execOut.stockTicker,'\0',sizeof(execOut.stockTicker));
	memcpy(execOut.stockTicker,order.stockTicker,sizeof(execOut.stockTicker));
	execOut.orderRefNum=order.orderRefNum;
	execOut.size=order.size;
	execOut.price=order.price;
	execOut.timeStamp=order.timeStamp;
}

void ExecutedOrderOutMsg::dump()
{
	std::cout<<"messageType="<<execOut.msgType<<"\n";
	std::cout<<"messageSize="<<execOut.msgSize<<"\n";
	std::cout<<"Ticker="<<this->getStockTicker()<<"\n";
	std::cout<<"timeStamp="<<this->getTimeStamp()<<"\n";
	std::cout<<"OrderRefNum="<<this->getOrderRefNum()<<"\n";
	std::cout<<"Size="<<this->getSize()<<"\n";
	std::cout<<"Price="<<this->getPrice()<<"\n";
}

void OrderCancelledInMsg::store(Order& order)
{
	order.timeStamp =this->getTimeStamp();
}

void OrderReducedOutMsg::copy(Order& order)
{
	memset(reduceOut.stockTicker,'\0',sizeof(reduceOut.stockTicker));
	memcpy(reduceOut.stockTicker,order.stockTicker,sizeof(reduceOut.stockTicker));
	reduceOut.orderRefNum=order.orderRefNum;
	reduceOut.size=order.size;
	reduceOut.timeStamp=order.timeStamp;
}

void OrderReducedOutMsg::dump()
{
	std::cout<<"messageType="<<reduceOut.msgType<<"\n";
	std::cout<<"messageSize="<<reduceOut.msgSize<<"\n";
	std::cout<<"Ticker="<<this->getStockTicker()<<"\n";
	std::cout<<"timeStamp="<<this->getTimeStamp()<<"\n";
	std::cout<<"OrderRefNum="<<this->getOrderRefNum()<<"\n";
	std::cout<<"Size="<<this->getSize()<<"\n";
}

Order OrderRepalcedInMsg::store(const char side, std::string& stockTicker)
{
	orderDetails.orderRefNum=this->getNewOrderRefNum();
	orderDetails.timeStamp =this->getTimeStamp();
	orderDetails.side=side;
	orderDetails.size =this->getSize();
	memset(orderDetails.stockTicker,'\0',sizeof(orderDetails.stockTicker));
	memcpy(orderDetails.stockTicker,stockTicker.c_str(),sizeof(orderDetails.stockTicker));
	orderDetails.price=this->getPrice();
	return orderDetails;
}

void OrderReplacedOutMsg::copy(OrderRepalcedInMsg& cpyMsg, std::string &ticker)
{
	replaceOut.orgOrderRefNum=cpyMsg.getOrgOrderRefNum();
	replaceOut.newOrderRefNum=cpyMsg.getNewOrderRefNum();
	replaceOut.newSize=cpyMsg.getSize();
	replaceOut.newPrice=cpyMsg.getPrice();
	memset(replaceOut.stockTicker,'\0',sizeof(replaceOut.stockTicker));
	memcpy(replaceOut.stockTicker,ticker.c_str(),sizeof(replaceOut.stockTicker));
	replaceOut.timeStamp=cpyMsg.getTimeStamp();
}

void OrderReplacedOutMsg::dump()
{
	std::cout<<"messageType="<<replaceOut.msgType<<"\n";
	std::cout<<"messageSize="<<replaceOut.msgSize<<"\n";
	std::cout<<"Ticker="<<this->getStockTicker()<<"\n";
	std::cout<<"timeStamp="<<this->getTimeStamp()<<"\n";
	std::cout<<"OrgOrderRefNum="<<this->getOrgOrderRefNum()<<"\n";
	std::cout<<"NewOrderRefNum="<<this->getNewOrderRefNum()<<"\n";
	std::cout<<"size="<<this->getSize()<<"\n";
	std::cout<<"Price="<<this->getPrice()<<"\n";
}
