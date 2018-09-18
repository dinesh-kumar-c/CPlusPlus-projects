This testing framework leverages the driver program *feed* & *input.in* binary file provided by HRT
*Running the binary*
./createTestInput
## make sure inp.asc is present in the same folder as in the format described below.


FileName: **feed**
- binary to run tests, provided as a part of the starter.zip package by HRT

FileName: **input.in**
- is the input file for the testing binary named "feed" ,  provided as a part of the starter.zip package by HRT but extended
   to contain all the input messages - AddOrder,OrderExecuted Message, Ordercancelled message & order replaced message & other
  test cases like messages straddled over separate UPD packets, multiple messages in the same UDP packet. 

FileName: **createtestinput.cc**
- this file creates the *input.in* as per the config file *inp.asc*
  The code has the logic to create the following test input according the details from config file
  - Add Order Message
  - Order Executed Message
  			- Executed Order message has the out of sequence UDP packets test case.
  			where the UDP packets arrive in the order lastSequence+2, lastSequence+1,lastSequence
  			
  - Order Cancelled message &
  - Order Replaced message
  - Any Add Order messages created has the following also test input created
            - Add Order message straddled over separate UPD packets
            - multiple Add Order messages in the same UDP packet. 

  To compile this file use - *g++ createtestinput -o createtestinput -I<path for messages.h file> -std=c++11*                  

FileName: **inp.asc**
- is the config file which has the order details in the format defined the specifications
  for example a AddOrdder message would be defined as
  msgType,TimeStamp,OrderReferenceNumber,Side,Size,StockTicker,price
  A,100123232222,1,B,300,VOD,123

