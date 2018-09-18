#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include "messages.h"

using namespace messages;
typedef std::unordered_map<uint64_t,Order> uoMap;

class Parser
{
  private:
	std::string opfile;
	int dateFile;
	FILE *filedescriptor;
	uoMap orderBook;
	std::map<uint32_t,const char *> seqNumQ;
	uint32_t lastSeqNum;
	time_t m_DayRefTime;

  public:
    // date - the day on which the data being parsed was generated.
    // It is specified as an integer in the form yyyymmdd.
    // For instance, 18 Jun 2018 would be specified as 20180618.
    //
    // outputFilename - name of the file output events should be written to.
    Parser(int date, const std::string &outputFilename);
    ~Parser();


    // buf - points to a char buffer containing bytes from a single UDP packet.
    // len - length of the packet.
    void onUDPPacket(const char *buf, size_t len);
    uint32_t processUDPBuffer(const char *buf, int len);
    void onAddOrder(const char* message, FILE* fptr);
    void onCancelOrder(const char* message, FILE* fptr);
    void onExecutedOrder(const char* message, FILE* fptr);
    void onReplacedOrder(const char* message, FILE* fptr);

};

