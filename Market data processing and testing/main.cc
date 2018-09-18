#include "Parser.h"

#include <cstdio>
#include <cstdint>

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>

const char *inputFile = "test.in";

int main(int argc, char **argv)
{
    constexpr int currentDate = 20180612;
    Parser myParser(currentDate, "myTestFile");

    int fd = open(inputFile, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Couldn't open %s\n", inputFile);
        return 1;
    }

    char bigbuf[5000];int bytesread=0;
    while (read(fd, bigbuf, 2) != 0)
    {
    	printf("----------------------------- Driver main:reading UDPpacket!\n");
        uint16_t packetSize = htons(*(uint16_t *)bigbuf);
        bytesread = read(fd, bigbuf + 2, packetSize - 2);
        if (bytesread>0)
        	myParser.onUDPPacket(bigbuf, packetSize);
        else
        	printf("Read Error!!");
    }

    close(fd);

    return 0;
}
