/*Welcome, this is the final, ultimate program for the offensive robot. Here is the main setup.
Anything that's acting up in regards to something with Network in it's name, ask Allen for help.
List of things to do:  
      1. We have to set up at least 3 classes, that are simply called by the loop.
              a) Drive
              b) Intake
              c) Shooter
Besides all that the code needs more commenting for future programs to reference back when copying and pasting.

*/

#include <NetworkTable.h>
#include <PS2X_lib.h>
#include <PacketSerial.h>


NetworkTable network = NetworkTable(10, 10);
PacketSerial myPacketSerial;
PS2X ps2x;
bool blink_value = false;
unsigned long last_blink = 0;


//define logic pins for each motor
//motors are numbered from the front of the robot counter-clockwise
//1-4 are for the drivers. 5 is for the intake. 6 is for the shooter.
//A FIFTH ONE IS NEEDED!
int enable1 = 11;
int pow1 = 12;
int dir1 = 13;

int enable2 = 8;
int pow2 = 9;
int dir2 = 10;

int enable3 = 5;
int pow3 = 6;
int dir3 = 7;

int enable4 = 2;
int pow4 = 3;
int dir4 = 4;
/*
int enable5 = ;
int pow5 = ;
int dir5 = ;

int enable6 = ;
int pow6 = ;
int dir6 = ;
*/


void setup()
{
	    pinMode(13, OUTPUT);
	    ps2x.config_gamepad();
	    ps2x.read_gamepad();
	Serial.begin(115200);
	Serial1.begin(115200);
	myPacketSerial.setStream(&Serial1);
	    myPacketSerial.setPacketHandler([](const uint8_t* buffer, size_t size) {
	    network.processPacketFromSender(myPacketSerial, buffer, size);
    	});
    	network.setPS2(&ps2x);
  pinMode(enable1, OUTPUT);
  pinMode(pow1, OUTPUT);
  pinMode(dir1, OUTPUT);
  
  pinMode(enable2, OUTPUT);
  pinMode(pow2, OUTPUT);
  pinMode(dir2, OUTPUT);

  pinMode(enable3, OUTPUT);
  pinMode(pow3, OUTPUT);
  pinMode(dir3, OUTPUT);

  pinMode(enable4, OUTPUT);
  pinMode(pow4, OUTPUT);
  pinMode(dir4, OUTPUT); 
  
}

void ledService()
{
	// TODO: change led blink from timer to watchdog
	unsigned long temp = millis() - last_blink;
	if((temp > 500))
	{
		last_blink = millis();
		blink_value = !blink_value;
		digitalWrite(13, blink_value);
	}
}

void loop() {
	myPacketSerial.update();
	ledService();
 
  
	if (network.getLastPS2PacketTime() > 500 || ps2x.Button(PSB_CROSS))
	{
		digitalWrite(enable1, LOW);
		digitalWrite(enable2, LOW);
		digitalWrite(enable3, LOW);
		digitalWrite(enable4, LOW); 
  	} else {
		digitalWrite(enable1, HIGH);
		digitalWrite(enable2, HIGH);
		digitalWrite(enable3, HIGH);
		digitalWrite(enable4, HIGH); 
	}
  //get joystick readings, 0-255 centered on 128
  int LX = ps2x.Analog(PSS_LX);   //0=left   255=right
  int LY = ps2x.Analog(PSS_LY);   //0=forward   255=backward
  int RX = ps2x.Analog(PSS_RX);   //0=left   255=right
  int RY = ps2x.Analog(PSS_RY);   //0=forward   255=backward

  //be sure to add another for the 5th.
  int straight = 0;
  int side = 0;
  int turn = 0;
  
  //for debugging
  //display joystick readings

  //Determine straight factor
  if(LY <= 130 && LY >= 125)  //joystick is centered verticaly
  {
      //do not change straight factor
  }
  else if(LY < 125)   //joystick is forward
  {
    straight = -(128 - LY) * 100/127;   //straight factor scaled to 100 -
  }
  else if(LY > 130)   //joystick is backward
  {
    straight = (LY - 128) *  100/127;  //straight factor scaled to 100 +
  }

  //Determine side factor
  if(LX <= 130 && LX >= 125)    //joystick is centered horizontally
  {
      //do not change side factor
  }
  else if(LX < 125)   //joystick is left
  {
    side = -(127 - LX) * 100/125;   //side factor scaled to 100 -
  }
  else if(LX > 130)   //joystick is right
  {
    side = (LX - 127) *  100/127;   //side factor scaled to 100 +
  }

  //Determine tunring factor
  if(RX <= 130 && RX >= 125)    //joystick is centered horizontally
  {
      //do not change turn factor
  }
  else if(RX < 125)   //joystick is left
  {
    turn = (127 - RX) *  100/127;    //turn factor scaled to 100 +
  }
  else if(RX > 130)   //joystick is right
  {
    turn = -(RX - 127) *  100/127;    //side factor scaled to 100 -
  }

  //set each motor's direction and speed
  //speed1
  int speed1 = straight - side + turn;
  if(speed1 >= 0)
  {
    digitalWrite(dir1, HIGH);
  }
  else
  {
    digitalWrite(dir1, LOW);
  }
  
  speed1 = speed1 * 2.55;
  if(speed1 < 0)
  {
    speed1 = -speed1;
  }
  if(speed1 > 255)
  {
    analogWrite(pow1, 255);
  }
  else
  {
    analogWrite(pow1, speed1);
  }
  
  //speed2
  int speed2 = straight + side + turn;
  if(speed2 >= 0)
  {
    digitalWrite(dir2, HIGH);
  }
  else
  {
    digitalWrite(dir2, LOW);
  }

  speed2 = speed2 * 2.55;
  if(speed2 < 0)
  {
    speed2 = -speed2;
  }

  if(speed2 > 255)
  {
    analogWrite(pow2, 255);
  }
  else
  {
    analogWrite(pow2, speed2);
  }
  
  int speed3 = -straight + side + turn;
  if(speed3 >= 0)
  {
    digitalWrite(dir3, HIGH);
  }
  else
  {
    digitalWrite(dir3, LOW);
  }
  
  speed3 = speed3 * 2.55;
  if(speed3 < 0)
  {
    speed3 = -speed3;
  }
  if(speed3 > 255)
  {
    analogWrite(pow3, 255);
  }
  else
  {
    analogWrite(pow3, speed3);
  }

  //speed 4
  int speed4 = -straight - side + turn;
  if(speed4 >= 0)
  {
    digitalWrite(dir4, HIGH);
  }
  else
  {
    digitalWrite(dir4, LOW);
  }
  
  speed4 = speed4 * 2.55;
  if(speed4 < 0)
  {
    speed4 = -speed4;
  }
  if(speed4 > 255)
  {
    analogWrite(pow4, 255);
  }
  else
  {
    analogWrite(pow4, speed2);
  }
		Serial.println(speed1);
		Serial.println();
}


