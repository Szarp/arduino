// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: HC SR501 PIR  logger  for imwave
//
//
// HISTORY:
//

#ifndef imPIR501_h
#define imPIT501_h

#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"

// Setup a oneWire instance to communicate with ANY OneWire devices


#define maxPirTab 8
 byte pirTab[maxPirTab];
 byte pirCount;


void SetupPIR501()
{
  pinMode(PIR_BUS,INPUT);
  DBGINFO("bus:");
  DBGINFO(PIR_BUS);
  pirCount=0;

}




bool DataPIR501(IMFrame &frame)
{
//   IMFrameData *data =frame.Data();
   int PIR=analogRead(PIR_BUS);
   byte pir= PIR>600?1:0;
    DBGINFO("|");
//    DBGINFO(PIR);
//    DBGINFO("|");
//    DBGINFO("*");
   if (pirCount<maxPirTab){
     pirTab[pirCount]=pir;
     pirCount++;
     return false;
   }  
   for(unsigned short i=0 ; i<maxPirTab ; i++)
      frame.Body[i]=pirTab[i];
   pirCount=0;

    DBGINFO(pirCount);
    DBGINFO("pir ");
    return true;


}





#endif
//
// END OF FILE
//
