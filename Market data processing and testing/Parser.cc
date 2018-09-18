#include "Parser.h"

#include <string>
#include <cstdio>
#include <iostream>

using namespace messages;

Parser::Parser(int date, const std::string &filename):opfile(filename),dateFile(date),\
													  lastSeqNum(0)
{
	if ((filedescriptor = fopen(opfile.c_str(),"wb")) == NULL)
	{
	   printf("Error! opening file");
	   // Program exits if the file pointer returns NULL.
	   exit(1);
	}

    //calculate the epoch time at start from the start of the day
	struct tm tmp;
	time(&m_DayRefTime);
	localtime_r (&m_DayRefTime, &tmp);
	m_DayRefTime -= ((tmp.tm_hour*60*60) + (tmp.tm_min *60) + tmp.tm_sec);
	//std::cout<<"start of the day time is:"<<m_DayRefTime;
}

Parser::~Parser()
{
	fclose(filedescriptor);
}

void Parser::onUDPPacket(const char *buf, size_t len)
{
    try
    {
    	//std::cout<<"UDP pkt Len: "<<len<<".\n";
		UdpPacket pkt(buf,len);
		uint32_t seqNumber =pkt.getPacketHeader().seqNum;

		//The first packet processed by your parser
		//should be the packet with sequence number 1
		//push it into Q if the seqNum is non-sequential
		std::cout<<"Received UDP Seq [" << seqNumber << "]\n";
		if (lastSeqNum>1 && seqNumber!=lastSeqNum+1)
		{
			if (seqNumber > lastSeqNum)
			{
				char* mem = new char[len];
				memcpy(mem,buf,len);
				seqNumQ.insert(std::make_pair(seqNumber, mem));
				//std::cout<<"########### Inserted into Q\n########### ";
			}
			return;
		}


		lastSeqNum = seqNumber;
		//static buffer to process straddle messages
		static char inbuf[1024];
		static uint32_t Offset=0;

		//copy input buffer the static buffer above
		//without the UDP header
		memcpy(inbuf+Offset,buf+sizeof(UdpHeader), len-sizeof(UdpHeader));
		Offset = processUDPBuffer(inbuf,Offset+len-sizeof(UdpHeader));
		memmove(inbuf,buf+len-Offset,Offset);

		//now if the queue where we sent in out-of-sequence messages
		//is not empty we should iterate thro it and process the UDP
		//packets
		if ( !seqNumQ.empty() )
		{
			for (auto it=seqNumQ.begin(); it!=seqNumQ.end();)
			{
				const char * buffer= it->second;
				uint16_t packetSize = htons(*(uint16_t *)buffer);
				uint32_t packetSeq = htonl(*(uint32_t*)(buffer+sizeof(uint16_t)));;
				//std::cout<<"########### Iterating thro Q\n";
				std::cout<<"\n from Q: packetsize["<<packetSize<<"]  SeqNo:["<<packetSeq<<"]\n";

				if (packetSeq == lastSeqNum+1)
				{
					lastSeqNum = packetSeq;
					//static buffer to process straddle messages
					static char inbuf[1024];
					static uint32_t Offset=0;

					//copy input buffer the static buffer above
					//without the UDP header
					memcpy(inbuf+Offset,buffer+sizeof(UdpHeader), packetSize-sizeof(UdpHeader));
					Offset = processUDPBuffer(inbuf,Offset+packetSize-sizeof(UdpHeader));
					memmove(inbuf,buffer+packetSize-Offset,Offset);

					auto ittmp=it;
					it++;
					delete[] buffer;
					seqNumQ.erase(ittmp);
				}
				else
					break;

			}
		}
		else
		{//The Q is empty no out of sequence packet yet
		}
    }
    catch (std::bad_alloc& error)
    {
      std::cerr << "out-of-memory while resolving SeqNum gap: " << error.what() << '\n';
    }
    catch (...)
    {//relevent catches can be added
    	std::cerr<<"!!!!!!!!!!Error processing UDP packet!!!!!!!\n";
    }
}

uint32_t Parser::processUDPBuffer(const char *buf, int len)
{
    try
    {
    	uint32_t leftOverBytes= len;
		bool moreToProcess=true;

		//while bytes to be processed is greater that any message
		//header, like AddOrder, CancelOrder messages' header
		//which contains the message Type
		while(moreToProcess && leftOverBytes >= 1)
		{
			char messageType = *buf;
			const char* message=buf;
			//std::cout<<"Message type is "<<messageType<<".\n";
			switch(messageType)
			{
				case 'A'://process addorder
				{
					if (leftOverBytes >= ADDORDERSIZE )
					{
						//std::cout<<"----------------------Received Add order message.\n";
						onAddOrder(message,filedescriptor);
						leftOverBytes-=ADDORDERSIZE;
						buf+=ADDORDERSIZE;
					}
					else
					{
						moreToProcess=false;
					}
					break;
				}
				case 'E'://process executionorder
				{
					if (leftOverBytes >= EXECUTIONORDERSIZE )
					{
						//std::cout<<"----------------------Received Executed order message.\n";
						onExecutedOrder(message,filedescriptor);
						leftOverBytes-=EXECUTIONORDERSIZE;
						buf+=EXECUTIONORDERSIZE;

					}
					else
					{
						moreToProcess=false;
					}
					break;
				}
				case 'X'://process cancelorder
				{
					if (leftOverBytes >= CANCELORDERSIZE )
					{
						//std::cout<<"----------------------Received Cancelled order message.\n";
						onCancelOrder(message,filedescriptor);
						leftOverBytes-=CANCELORDERSIZE;
						buf+=CANCELORDERSIZE;
					}
					else
					{
						moreToProcess=false;
					}
					break;
				}
				case 'R'://process replaceorder
				{
					if (leftOverBytes >= REPLACEORDERSIZE )
					{
						//std::cout<<"----------------------Received Replaced order message.\n";
						onReplacedOrder(message,filedescriptor);
						leftOverBytes-=REPLACEORDERSIZE;
						buf+=REPLACEORDERSIZE;

					}
					else
					{
						moreToProcess=false;
					}
					break;
				}
				default:
					std::cerr<<"Invalid Message Type received:"<<messageType<<"\n";
			}
			//std::cout<<"ProcessBuffer:leftOverBytes:"<<leftOverBytes<<"\n";
		}
		return leftOverBytes;
    }
    catch (...)
    {
    	std::cerr<<"Error Processing UDP packet!!!!!\n";
    }
}

void Parser::onAddOrder(const char * message, FILE* fptr)
{
	try
	{
		AddOrderInMsg msgin(message);
		//std::cout<<"---- input message structure ....\n";
		Order order = msgin.store();
		orderBook.emplace(order.orderRefNum,order);

		AddOrderOutMsg msgout (m_DayRefTime);
		//Copying to output message structure
		msgout.copy(msgin);

		//std::cout<<"---- output struct before writing to the file ....	\n";
		//msgout.dump();

		//std::cout<<"---- writing to output file---- \n";
		fwrite((void*)msgout.getAddOurOrderStruct(), sizeof(AddOrderOut), 1, fptr);
	}
	catch (...)
	{
		//relevent catches can be added
		std::cerr<<"Error Processing onAddOrder mesasage!!!!!\n";
	}
}

void Parser::onExecutedOrder(const char * message, FILE* fptr)
{
	try
	{
		ExecutedOrderInMsg msgin(message);

		//find orderRefNum in the Orderbook map
		auto found = orderBook.find(msgin.getOrderRefNum());
		if ( found == orderBook.end() )
		{
			std::cerr<<"!!!!Order not found in orderbook!!!!!!\n";
		}
		else
		{
			//std::cout << "Order found \n";
		}
		//std::cout<<"---- input message structure ....\n";
		msgin.store(found->second);

		ExecutedOrderOutMsg msgout(m_DayRefTime);
		//Copying to output message structure
		msgout.copy(found->second);

		//std::cout<<"---- output struct before writing to the file ....	\n";
		//msgout.dump();

		//std::cout<<"writing to output file\n";
		fwrite((void*)msgout.getExecutedOrderStruct(), sizeof(OrderExecutedOut), 1, fptr);

	}
	catch (...)
	{//relevent catches can be added
		std::cerr<<"Error Processing ExecutedOrder mesasage!!!!!\n";
	}
}


void Parser::onCancelOrder(const char * message, FILE* fptr)
{
	try
	{
		OrderCancelledInMsg msgin(message);
		//std::cout<<"---- input message structure ....\n";
		auto found = orderBook.find(msgin.getOrderRefNum());
		if ( found == orderBook.end() )
		{
			std::cerr<<"!!!!Order not found in orderbook!!!!!!\n";
		}
		else
		{
			//std::cout << "Order found \n";
		}
		msgin.store(found->second);

		OrderReducedOutMsg msgout(m_DayRefTime);
		//Copying to output message structure
		msgout.copy(found->second);

		//std::cout<<"---- output struct before writing to the file ....	\n";
		//msgout.dump();

		//std::cout<<"writing to output file\n";
		fwrite((void*)msgout.getReducedOrderStruct(), sizeof(OrderReducedOut), 1, fptr);

		//erase the order from Orderbook
		orderBook.erase(found);
	}
	catch (...)
	{//relevent catches can be added
		std::cerr<<"Error Processing CancelledOrder mesasage!!!!!\n";
	}
}

void Parser::onReplacedOrder(const char * message, FILE* fptr)
{
	try
	{
		OrderRepalcedInMsg msgin(message);

		auto found = orderBook.find(msgin.getOrgOrderRefNum());
		if ( found == orderBook.end() )
		{
			std::cerr<<"!!!!Order not found in orderbook!!!!!!\n";
		}
		else
		{
			//std::cout << "Order found \n";
		}
		char side = found->second.size;
		std::string stockTicker(found->second.stockTicker);

		//now tht we have all the detials from org order
		//delete original order ref key in Orderbook
		orderBook.erase (found);

		//std::cout<<"---- input message structure ....\n";
		Order order = msgin.store(side, stockTicker);

		//enter the new order
		orderBook.emplace(order.orderRefNum,order);

		OrderReplacedOutMsg msgout(m_DayRefTime);
		msgout.copy(msgin,stockTicker);
		//std::cout<<"---- output struct before writing to the file ....	\n";
		//msgout.dump();

		//std::cout<<"writing to output file\n";
		fwrite((void*)msgout.getRepalcedOrderStruct(), sizeof(OrderReplacedOutMsg), 1, fptr);

	}
	catch (...)
	{//relevent catches can be added
		std::cerr<<"Error Processing CancelledOrder mesasage!!!!!\n";
	}
}



