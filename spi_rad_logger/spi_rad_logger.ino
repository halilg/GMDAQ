/*Serial Communication for Radiation Detector Arduino Compatible DIY Kit ver 2.01 or higher
* HOW TO CONNECT GEIGER KIT?
* The kit 3 wires that should be connected to Arduino UNO board: 5V, GND and INT. PullUp resistor is included on
* kit PCB. Connect INT wire to Digital Pin#2 (INT0), 5V to 5V, GND to GND. Then connect the Arduino with
* USB cable to the computer and upload this sketch. 
*/


#include <SPI.h>

void tube_impulse(){       //subprocedure for capturing events from Geiger Kit
  Serial.println(micros());
}

void setup(){             //setup subprocedure
    Serial.begin(115200);
    
      // wait until some data comes down the line
      while (1){
          if (Serial.available() > 0) {
                // Let me introduce myself
                Serial.println("!IAM AUNO");
                break;
           }
        }  
  
    attachInterrupt(0, tube_impulse, FALLING); //define external interrupts   
}

void loop(){                                 //main cycle
 ;  
}

