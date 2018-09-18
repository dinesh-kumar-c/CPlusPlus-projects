/*This executable creates test input file from the config file
  mentioned global var 'testConfigFile'. The config should contain every
  message as 1 line in the format of the struct mentioned in messages.h
  example:A,1000001,1,B,100,VOD,1234556,# */

#include <stdio.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <messages.h>
#include <sys/types.h>
#include <stdint.h>
#include <iostream>
#include <boost/tokenizer.hpp>
using namespace messages;

const char *testConfigFile = "inp.asc";
const char *outputFile = "test.in";

int main()
{
	FILE *fp, *wp;
	fp = fopen(testConfigFile, "r");
	wp = fopen(outputFile, "w");
	char buff[256];
	if ( (wp == (FILE*) NULL) )
	{
		printf("Unable to open file to Write!!!!\n");
		exit(1);
	}

	if ( (fp != (FILE*) NULL) )
	{
		printf("File opened\n");
		uint16_t i=0;
		while ( fgets(buff, 256, fp ) )
		{
			i++;
			int arrsize = 0;
			char arr[100][100];
			char *pch= strtok(buff, ",");
			strcpy(arr[arrsize], pch);
			printf("Value is [%s][%d]", arr[arrsize], arrsize);
			while ( pch != NULL )
			{
				arrsize++;
				pch = strtok(NULL,",");
				if ( pch != NULL)
				{
					strcpy(arr[arrsize], pch);
					printf("Valus is [%s][%d]", arr[arrsize], arrsize);
				}
			}
			printf("Done\n");
			char msgType=buff[0];
			UdpHeader udpheader;
			uint32_t sn = ntohl(i);
			udpheader.seqNum = sn;

			switch (msgType)
			{
				case 'A':
						{
							udpheader.packetSize=ntohs(98);
							printf( "mesage is A\n");
							AddOrderIn add;
							add.msgType='A';
							uint64_t ti = std::stoll(arr[1]);
							add.timeStamp= be64toh(ti);
							uint64_t ord = std::stoll(arr[2]);
							add.orderRefNum = be64toh(ord);
							add.side = arr[3][0];
							add.size = ntohl(atol(arr[4]));
							memset(add.stockTicker, ' ', sizeof(add.stockTicker));
							memcpy(add.stockTicker, arr[5], strlen(arr[5]));
							//memcpy(add.stockTicker, arr[5], sizeof(add.stockTicker));
							add.price=ntohl(atol(arr[6]));
							fwrite((void*) & udpheader, sizeof(udpheader), 1,wp);
							fwrite((void*) & add, sizeof(add), 1,wp);
							fwrite((void*) & add, sizeof(add), 1,wp);
							fwrite((void*) & add, sizeof(add)-10, 1,wp);
							udpheader.packetSize=ntohs(16);
							i++;
							uint32_t sn = ntohl(i);
							udpheader.seqNum = sn;
							fwrite((void*) & udpheader, sizeof(udpheader), 1,wp);
							char *tmp = (char*)&add;
							fwrite((void*) tmp+24, 10, 1,wp);
						}
						break;
				case 'E':
						{
							sn = ntohl(i+2);
							udpheader.seqNum = sn;
							udpheader.packetSize=ntohs(27);
							printf( "mesage is E\n");
							OrderExecutedIn exec1;
							exec1.msgType='E';
							uint64_t ti = std::stoll(arr[1]);
							exec1.timeStamp= be64toh(ti);
							uint64_t ord = std::stoll(arr[2]);
							exec1.orderRefNum = be64toh(ord);
							exec1.size = ntohl(atol(arr[4]));
							fwrite((void*) & udpheader, sizeof(udpheader), 1,wp);
							fwrite((void*) & exec1, sizeof(exec1), 1,wp);
							uint32_t sn = ntohl(i+2);

							sn = ntohl(i+1);
							udpheader.seqNum = sn;
							udpheader.packetSize=ntohs(27);
							printf( "mesage is E\n");
							OrderExecutedIn exec2;
							exec2.msgType='E';
							uint64_t ti1 = std::stoll(arr[1]);
							exec2.timeStamp= be64toh(ti1);
							uint64_t ord1 = std::stoll(arr[2]);
							exec2.orderRefNum = be64toh(ord1);
							exec2.size = ntohl(atol(arr[4]));
							fwrite((void*) & udpheader, sizeof(udpheader), 1,wp);
							fwrite((void*) & exec2, sizeof(exec2), 1,wp);

						    sn = ntohl(i);
							udpheader.seqNum = sn;
							udpheader.packetSize=ntohs(27);
							printf( "mesage is E\n");
							OrderExecutedIn exec3;
							exec3.msgType='E';
							uint64_t ti2 = std::stoll(arr[1]);
							exec3.timeStamp= be64toh(ti2);
							uint64_t ord3 = std::stoll(arr[2]);
							exec3.orderRefNum = be64toh(ord3);
							exec3.size = ntohl(atol(arr[4]));
							fwrite((void*) & udpheader, sizeof(udpheader), 1,wp);
							fwrite((void*) & exec3, sizeof(exec3), 1,wp);
							i+=2;
						}
						break;
				case 'X':
						{
							udpheader.packetSize=ntohs(27);
							printf( "mesage is X\n");
							OrderCancelledIn cxl;
							cxl.msgType='X';
							uint64_t ti = std::stoll(arr[1]);
							cxl.timeStamp= be64toh(ti);
							uint64_t ord = std::stoll(arr[2]);
							cxl.orderRefNum = be64toh(ord);
							cxl.size = ntohl(atol(arr[4]));
							fwrite((void*) & udpheader, sizeof(udpheader), 1,wp);
							fwrite((void*) & cxl, sizeof(cxl), 1,wp);
						}
						break;

				case 'R':
						{
							udpheader.packetSize=ntohs(39);
							printf( "mesage is R\n");
							OrderReplacedIn rep;
							rep.msgType='R';
							uint64_t ti = std::stoll(arr[1]);
							rep.timeStamp= be64toh(ti);
							uint64_t ord = std::stoll(arr[2]);
							rep.orginalOrderRefNum = be64toh(ord);
							uint64_t nord = std::stoll(arr[3]);
							rep.newOrderRefNum = be64toh(nord);
							rep.size = ntohl(atol(arr[4]));
							rep.price=ntohl(atol(arr[5]));
							fwrite((void*) & udpheader, sizeof(udpheader), 1,wp);
							fwrite((void*) & rep, sizeof(rep), 1,wp);
						}
						break;
				default:
					printf("Incorrect input line in config file!!\n");
			}
		}
	}
	else
	{
		printf("Unable to open file[%s]--error- [%s]\n", testConfigFile,strerror(errno));
	}

}
