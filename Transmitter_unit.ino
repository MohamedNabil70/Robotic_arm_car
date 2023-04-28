#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

#define VRX_PIN  A4 // Arduino pin connected to VRX pin
#define VRY_PIN  A5 // Arduino pin connected to VRY pin

RF24 radio(7, 8); // CSN, CE
//const byte address[6] = "00001";
const uint64_t pipeOut = 0xF9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xF9E8F0F0E1LL

struct morsal
{
int analog_x;
int analog_y;
int Pot_1;
int Pot_2;
int Pot_3;
int Pot_4;
};
                                            
struct morsal Data;

void setup() {
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  //radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
 
}

//This function is used to map 0-1023 joystick value to 0-254. hence 127 is the center value which we send.
//It also adjust the deadband in joystick.
//Jotstick  values range from 0-1023. But its center value is not always 511. It is little different.
//So we need to add some deadband to center value. in our case 500-530. Any value in this deadband range is mapped to center 127.


void loop() {

Data.analog_x = analogRead(VRX_PIN);
Data.analog_y = analogRead(VRY_PIN);
  
Data.Pot_1 = analogRead(A0);
Data.Pot_2 = analogRead(A1);
Data.Pot_3 = analogRead(A2);
Data.Pot_4 = analogRead(A3);

radio.write(&Data, sizeof(morsal));

delay(10);

}
