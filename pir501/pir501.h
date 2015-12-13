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


#define maxPirTab 16
 byte pirTab[maxPirTab];
 byte pirCount;


void SetupPIR501()
{
  pinMode(PIR_BUS,INPUT_PULLUP);
  DBGINFO("bus:");
  DBGINFO(PIR_BUS);
  pirCount=0;

}




bool DataPIR501(IMFrame &frame)
{
//   IMFrameData *data =frame.Data();
   byte pir=digitalRead(PIR_BUS);
   if (pirCount<maxPirTab){
     pirTab[pirCount]=pir;
     pirCount++;
     return false;
   }  
   for(unsigned short i=0 ; i<maxPirTab ; i++)
      frame.Body[i]=pirTab[i];
   pirCount=0;

    DBGINFO(" pir ");
    return true;


}





#endif
//
// END OF FILE
//
