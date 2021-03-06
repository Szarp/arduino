
#include <imframe.h>
#define REQUIRESALARMS 0
#include <imatmega.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/wdt.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2







//#.define DBGLVL 2
#include "imdebug.h"
#include "ds18b20.h"





/******************************** Configuration *************************************/

#define MMAC 0x170000  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 7     //Type of device






/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imtimer.h"


IMCC1101  cc1101;
Transceiver trx;







void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}





void OnRead(byte state)
{
  DBGINFO(state);
  if (state==2)
  {
    return ;
  }
  trx.timer.doneListen();
}



void PrepareData()
{
   if (trx.Connected())
   {
       PrepareDS18B20();
   }  
}  

void SendData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
        static IMFrame frame;
        frame.Reset();
        long mm=millis();
        DataDS18B20(frame);
        DBGINFO(" :");
        DBGINFO(millis()-mm);
        

        DBGINFO("SendData ");
        trx.SendData(frame);
        trx.Transmit();
        ERRFLASH();
      } 

   } else {
     trx.ListenBroadcast();
   }

}



void ReceiveData()
{
  static IMFrame rxFrame;
      if (trx.GetFrame(rxFrame))
      {
        if (!trx.ParseFrame(rxFrame))
        {
          DBGINFO(" rxGET ");
        }
      }
      DBGINFO("\r\n");


}

void PrintStatus()
{
  DBGINFO("\r\n");
  DBGINFO(" Status ");
  trx.printStatus();
  DBGINFO("\r\n");

}
 

void stageloop(byte stage)
{
//   if (stage== STARTBROADCAST){
//    DBGINFO("stageloop=");  DBGINFO(millis());
//    DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST:  trx.ListenBroadcast();  PrepareData();    break;
    case STOPBROADCAST:  trx.Knock();      break;
    case STARTDATA: SendData();break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
    case IMTimer::IDDLESTAGE : {
     DBGINFO("***IDDLE DATA");
     DBGINFO(millis());

       ReceiveData();break;
     }
    case IMTimer::PERIOD : 
  //      ERRFLASH();
 //     PrintStatus();
    break;
      

    default:
    break;
  }
}





void setup()
{
  delay(1000);
  INITDBG();
  ERRLEDINIT();   ERRLEDOFF();
  IMMAC ad=SetupDS18B20();
  wdt_enable(WDTO_8S);

  interrupts ();
  randomSeed(analogRead(0)+internalrandom());
  trx.Init(cc1101);
  trx.myMAC=MMAC;
  trx.myMAC+=ad;
  
  //  DBGINFO(" MMAC ");  DBGINFO2(trx.myMAC,HEX);
  //      DBGINFO("  ");
//     DBGINFO2( trx.myMAC, HEX );
  trx.myDevice=MDEVICE;
  trx.onEvent=OnRead;
  trx.timer.onStage=stageloop;
  pciSetup(9);
  if (ad>0){
    ERRLEDON();
    delay(1000);
    ERRLEDOFF();
   } else{
    ERRLEDON();
    delay(300);
    ERRLEDOFF();
    delay(200);
    ERRLEDON();
    delay(300);
    ERRLEDOFF();
    reboot();

  }     
//  trx.TimerSetup();
//   DBGINFO("classtest Timer");  DBGINFO(IMTimer::ClassTest());
}

void loop()
{
  wdt_reset();
//  ERRFLASH();
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);


}
