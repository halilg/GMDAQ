// Requires the Timer One and 1 Wire libraries 

#include <OneWire.h>
//#include <TimerOne.h>

// This example uses the timer interrupt to blink an LED
// and also demonstrates how to share a variable between
// the interrupt and the main program.


const int led = LED_BUILTIN;  // the pin with a LED

#define DS18S20_ID 0x10
#define DS18B20_ID 0x28
float temp;
OneWire  ds(10);

boolean getTemperature(){
   byte i;
   byte present = 0;
   byte data[12];
   byte addr[8];
   //find a device
   if (!ds.search(addr)) {
     ds.reset_search();
     return false;
   }
   if (OneWire::crc8( addr, 7) != addr[7]) {
     return false;
   }
   if (addr[0] != DS18S20_ID && addr[0] != DS18B20_ID) {
     return false;
   }
   ds.reset();
   ds.select(addr);
   // Start conversion
   ds.write(0x44, 1);
   // Wait some time...
   delay(850);
   present = ds.reset();
   ds.select(addr);
   // Issue Read scratchpad command
   ds.write(0xBE);
   // Receive 9 bytes
   for ( i = 0; i < 9; i++) {
   data[i] = ds.read();
   }
   // Calculate temperature value
   temp = ( (data[1] << 8) + data[0] )*0.0625;
   return true;
 }

  
void setup(void)
{
  pinMode(led, OUTPUT);
//  Timer1.initialize(1000000);
//  Timer1.attachInterrupt(gmfhit); // blinkLED to run every 0.15 seconds  
  Serial.begin(115200);
}

void gmfhit(void)
{
  Serial.println(1);
}


void loop(void) {
  if (getTemperature()){
    Serial.print("T");
    Serial.println(temp);
    delay(7000);
  }
}



