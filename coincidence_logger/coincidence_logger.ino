/*Serial Communication for Radiation Detector Arduino Compatible DIY Kit ver 2.01 or higher
* HOW TO CONNECT GEIGER KIT?
* The kit 3 wires that should be connected to Arduino UNO board: 5V, GND and INT. PullUp resistor is included on
* kit PCB. Connect INT wire to Digital Pin#2 (INT0), 5V to 5V, GND to GND. Then connect the Arduino with
* USB cable to the computer and upload this sketch. 
*/


#include <SPI.h>
const int gm1 = 3;     // top GM module is conected to this pin
int gm1State = 0;         // variable for reading the pushbutton status

void tube_impulse(){       //subprocedure for capturing events from Geiger Kit
  
  gm1State = digitalRead(gm1);
  if (gm1State==0){
      Serial.println(micros());
  }  
  
}

void setup(){             //setup subprocedure
    pinMode(gm1, INPUT);
    Serial.begin(115200);
    // When the PC opens the serial port, arduino resets. 
    // Therefore this will be the first message the PC gets once it opens the port.
    // Let me introduce myself:
    
    Serial.println("!IAM AUNO");
    attachInterrupt(0, tube_impulse, FALLING); //define external interrupts   
}

void loop(){                                 //main cycle
 ;  
}

