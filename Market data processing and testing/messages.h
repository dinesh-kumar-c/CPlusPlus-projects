#pragma once
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <unordered_map>

#pragma pack(push,1)
namespace messages
{
	struct UdpHeader
	{
		uint16_t packetSize;
		uint32_t seqNum;
	};

	struct Order
	{
		uint64_t timeStamp;
		uint64_t orderRefNum;
		//uint64_t orginalOrderRefNum;
		char side;
		uint32_t size;
		uint32_t filled;
		char stockTicker[8];
		double price;
	};

	struct AddOrderIn
	{
		char msgType;
		uint64_t timeStamp;
		uint64_t orderRefNum;
		char side;
		uint32_t size;
		char stockTicker[8];
		uint32_t price;// implied with 4 decimal places
	};


	struct OrderExecutedIn
	{
		char msgType;
		uint64_t timeStamp;
		uint64_t orderRefNum;
		uint32_t size;
	};

	struct OrderCancelledIn
	{
		char msgType;
		uint64_t timeStamp;
		uint64_t orderRefNum;
		uint32_t size;
	};

	struct OrderReplacedIn
	{
		char msgType;
		uint64_t timeStamp;
		uint64_t orginalOrderRefNum;
		uint64_t newOrderRefNum;
		uint32_t size;
		uint32_t price;// implied with 4 decimal places
	};

	struct AddOrderOut
	{
		uint16_t msgType;
		uint16_t msgSize;
		char stockTicker[8];
		uint64_t timeStamp;
		uint64_t orderRefNum;
		char side;
		char padding[3];//must be filled with 0x00
		uint32_t size;
		double price; //implied with 4 decimal places
	};

	struct OrderExecutedOut
	{
		uint16_t msgType;
		uint16_t msgSize;
		char stockTicker[8];
		uint64_t timeStamp;
		uint64_t orderRefNum;
		uint32_t size;
		double price; //implied with 4 decimal places
	};

	struct OrderReducedOut
	{
		uint16_t msgType;
		uint16_t msgSize;
		char stockTicker[8];
		uint64_t timeStamp;
		uint64_t orderRefNum;
		uint32_t size;
	};

	struct OrderReplacedOut
	{
		uint16_t msgType;
		uint16_t msgSize;
		char stockTicker[8];
		uint64_t timeStamp;
		uint64_t orgOrderRefNum;
		uint64_t newOrderRefNum;
		uint32_t newSize;
		double newPrice; //implied with 4 decimal places
	};

	#pragma pack (pop)

	static uint32_t ADDORDERSIZE= sizeof(AddOrderIn);
	static uint32_t EXECUTIONORDERSIZE= sizeof(OrderExecutedIn);
	static uint32_t CANCELORDERSIZE= sizeof(OrderCancelledIn);
	static uint32_t REPLACEORDERSIZE= sizeof(OrderReplacedIn);

	class UdpPacket
	{
	private:
		UdpHeader packetHeader;
		const char* buffer;
	public:
		UdpPacket(const char* buff, int len)
		{
			packetHeader.packetSize=len;
			packetHeader.seqNum=htonl(*(uint32_t*)(buff+sizeof(packetHeader.packetSize)));
			//packetHeader.seqNum=htonl(*(uint32_t*)(buff+2));
			buffer=buff;
		}
		//UdpPacket(const UdpPacket&){packetHeader.packetSize=0;packetHeader.seqNum=0;)
		UdpHeader& getPacketHeader()
		{
			return packetHeader;
		}
		uint32_t getSeqNum(){return packetHeader.seqNum;}

		//TODO:The payload carried by these packets is made up
		//of zero or more messages

		//TODO:Messages can straddle packet boundaries.

		const char* getPayload()
		{
			return (buffer + sizeof(UdpHeader));//does not work w/o pragma pack
			//return (buffer + (sizeof(packetHeader.packetSize)+sizeof(packetHeader.seqNum)));
			//return (buffer + 6);
		}
		uint16_t getPayloadSize()
		{
			return (packetHeader.packetSize-sizeof(UdpHeader));
		}
	};

	class AddOrderInMsg
	{
	  private:
		const char * buffer;
		Order orderDetails;

	  public:
		AddOrderInMsg(const char* buff)
		{
			buffer=buff;
		}

		uint64_t hostToNet64(uint64_t value)
		{
			const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
			const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));
			return (static_cast<uint64_t>(low_part) << 32) | high_part;
		}

		uint64_t getTimeStamp()
		{
			return hostToNet64(*(uint64_t*)(buffer+1));
		}
		uint64_t getOrderRefNum()
		{
			return (hostToNet64(*(uint64_t*)(buffer+9)));
		}

		char getSide()
		{
			return *(buffer+17);
		}

		uint32_t getSize()
		{
			return (htonl(*(uint32_t*)(buffer+18)));
		}

		std::string getStockTicker() const
		{

			char tmp1[8];
			char* tmp = &tmp1[7];
			strncpy(tmp1,buffer+22,sizeof(tmp1));
			while ( *tmp == ' ' )
			{
				*tmp='\0';
				tmp--;
			}
			std::string returnString(tmp1);
			return returnString;
		}

		double getPrice()
		{
			double returnPrice=(htonl(*(uint32_t*)(buffer + 30)))/10000.00;
			//printf("Actual Price %0.4f\n", returnPrice);
			return returnPrice;
		}

		Order store();
	};

	class AddOrderOutMsg
	{
	  private:
		AddOrderOut addOut;
		time_t  m_DayRefTime;

	  public:
		AddOrderOutMsg(time_t m_DayRefTimein):m_DayRefTime(m_DayRefTimein)
		{
			addOut.msgType=0x01;
			addOut.msgSize=44;
			memset(addOut.stockTicker,'\0',sizeof(addOut.stockTicker));
			memset(addOut.padding,'0',sizeof(addOut.padding));
		}

		uint64_t getNanoSecEpoch(uint64_t microsec)
		{
			struct timespec d1;
			d1.tv_sec= m_DayRefTime + (microsec/1000000);
			d1.tv_nsec= (microsec%1000000)*1000;
			return ((d1.tv_sec * 1000000000) + d1.tv_nsec);
		}
		void copy(AddOrderInMsg& cpyMsg);

		AddOrderOut* getAddOurOrderStruct()
		{
			return &addOut;
		}

		uint64_t getTimeStamp()
		{
			return getNanoSecEpoch (addOut.timeStamp);
		}
		uint64_t getOrderRefNum()
		{
			return addOut.orderRefNum;
		}

		char getSide()
		{
			return addOut.side;
		}

		uint32_t getSize()
		{
			return addOut.size;
		}

		std::string getStockTicker() const
		{
			std::string returnString(addOut.stockTicker);
			return returnString;
		}
		double getPrice()
		{
			return addOut.price;
		}
		void dump();
	};

	class ExecutedOrderInMsg
	{
	  private:
		const char * buffer;
		Order orderDetails;

	  public:
		ExecutedOrderInMsg(const char* buff)
		{
			buffer=buff;
		}

		uint64_t hostToNet64(uint64_t value)
		{
			const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
			const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));
			return (static_cast<uint64_t>(low_part) << 32) | high_part;
		}

		uint64_t getTimeStamp()
		{
			return (hostToNet64(*(uint64_t*)(buffer+1)));
		}

		uint64_t getOrderRefNum()
		{
			return (hostToNet64(*(uint64_t*)(buffer+9)));
		}

		uint32_t getSize()
		{
			return (htonl(*(uint32_t*)(buffer+17)));
		}

		void store(Order& order);
	};

	class ExecutedOrderOutMsg
	{
	  private:
		OrderExecutedOut execOut;
		time_t m_DayRefTime;

	  public:
		ExecutedOrderOutMsg(time_t m_DayRefTimein): m_DayRefTime( m_DayRefTimein)
		{
			execOut.msgType=0x02;
			execOut.msgSize=40;
		}
		uint64_t getNanoSecEpoch(uint64_t microsec)
		{
			struct timespec d1;
			d1.tv_sec= m_DayRefTime + (microsec/1000000);
			d1.tv_nsec= (microsec%1000000)*1000;
			return ((d1.tv_sec * 1000000000) + d1.tv_nsec);
		}
		void copy(Order& order);

		OrderExecutedOut* getExecutedOrderStruct()
		{
			return &execOut;
		}

		uint64_t getTimeStamp()
		{
			return getNanoSecEpoch(execOut.timeStamp);
		}
		uint64_t getOrderRefNum()
		{
			return execOut.orderRefNum;
		}

		uint32_t getSize()
		{
			return execOut.size;
		}

		std::string getStockTicker() const
		{
			std::string returnString(execOut.stockTicker);
			return returnString;
		}
		double getPrice()
		{
			return execOut.price;
		}
		void dump();
	};

	class OrderCancelledInMsg
	{
	  private:
		const char * buffer;
		Order orderDetails;

	  public:
		OrderCancelledInMsg(const char* buff)
		{
			buffer=buff;
		}

		uint64_t hostToNet64(uint64_t value)
		{
			const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
			const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));
			return (static_cast<uint64_t>(low_part) << 32) | high_part;
		}

		uint64_t getTimeStamp()
		{
			return (hostToNet64(*(uint64_t*)(buffer+1)));
		}

		uint64_t getOrderRefNum()
		{
			return (hostToNet64(*(uint64_t*)(buffer+9)));
		}

		uint32_t getSize()
		{
			return (htonl(*(uint32_t*)(buffer+17)));
		}

		void store(Order& order);
	};

	class OrderReducedOutMsg
	{
	  private:
		OrderReducedOut reduceOut;
		time_t m_DayRefTime;

	  public:
		OrderReducedOutMsg(time_t m_DayRefTimein):m_DayRefTime(m_DayRefTimein)
		{
			reduceOut.msgType=0x03;
			reduceOut.msgSize=32;
		}
		void copy(Order& order);

		uint64_t getNanoSecEpoch(uint64_t microsec)
		{
			struct timespec d1;
			d1.tv_sec= m_DayRefTime + (microsec/1000000);
			d1.tv_nsec= (microsec%1000000)*1000;
			return ((d1.tv_sec * 1000000000) + d1.tv_nsec);
		}

		OrderReducedOut* getReducedOrderStruct()
		{
			return &reduceOut;
		}

		uint64_t getTimeStamp()
		{
			return getNanoSecEpoch(reduceOut.timeStamp);
		}
		uint64_t getOrderRefNum()
		{
			return reduceOut.orderRefNum;
		}

		uint32_t getSize()
		{
			return reduceOut.size;
		}

		std::string getStockTicker() const
		{
			std::string returnString(reduceOut.stockTicker);
			return returnString;
		}
		void dump();
	};

	class OrderRepalcedInMsg
	{
	  private:
		const char * buffer;
		Order orderDetails;

	  public:
		OrderRepalcedInMsg(const char* buff)
		{
			buffer=buff;
		}

		uint64_t hostToNet64(uint64_t value)
		{
			const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
			const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));
			return (static_cast<uint64_t>(low_part) << 32) | high_part;
		}

		uint64_t getTimeStamp()
		{
			return (hostToNet64(*(uint64_t*)(buffer+1)));
		}

		uint64_t getOrgOrderRefNum()
		{
			return (hostToNet64(*(uint64_t*)(buffer+9)));
		}

		uint64_t getNewOrderRefNum()
		{
			return (hostToNet64(*(uint64_t*)(buffer+17)));
		}

		uint32_t getSize()
		{
			return (htonl(*(uint32_t*)(buffer+25)));
		}

		double getPrice()
		{
			return (htonl(*(uint32_t*)(buffer+29)))/10000.00;
		}

		Order store(const char side, std::string& stockTicker);
	};

	class OrderReplacedOutMsg
	{
	  private:
		OrderReplacedOut replaceOut;
		time_t m_DayRefTime;

	  public:
		OrderReplacedOutMsg(time_t m_DayRefTimein):m_DayRefTime(m_DayRefTimein)
		{
			replaceOut.msgType=0x04;
			replaceOut.msgSize=48;
		}
		uint64_t getNanoSecEpoch(uint64_t microsec)
		{
			struct timespec d1;
			d1.tv_sec= m_DayRefTime + (microsec/1000000);
			d1.tv_nsec= (microsec%1000000)*1000;
			return ((d1.tv_sec * 1000000000) + d1.tv_nsec);
		}

		OrderReplacedOut* getRepalcedOrderStruct()
		{
			return &replaceOut;
		}

		uint64_t getTimeStamp()
		{
			return replaceOut.timeStamp;
		}
		uint64_t getOrgOrderRefNum()
		{
			return replaceOut.orgOrderRefNum;
		}

		uint64_t getNewOrderRefNum()
		{
			return replaceOut.newOrderRefNum;
		}

		uint32_t getSize()
		{
			return replaceOut.newSize;
		}

		double getPrice()
		{
			return replaceOut.newPrice;
		}

		std::string getStockTicker() const
		{
			std::string returnString(replaceOut.stockTicker);
			return returnString;
		}
		void dump();
		void copy(OrderRepalcedInMsg& cpyMsg, std::string& ticker);
	};

}
