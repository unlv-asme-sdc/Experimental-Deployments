#include "NetworkTable.h"
#include "PS2X_lib.h"
#include <PacketSerial.h>

NetworkTable network = NetworkTable(10, 10);
PacketSerial myPacketSerial;
PS2X ps2x;
bool blink_value = false;
unsigned long last_blink = 0;

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

void setup()
{
	pinMode(13, OUTPUT);


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
	if(temp > 500)
	{
		last_blink = millis();
		blink_value = !blink_value;
		digitalWrite(13, blink_value);
	}
}

void loop() {
	  myPacketSerial.update();
    ledService();

    digitalWrite(enable1, HIGH);
    digitalWrite(enable2, HIGH);
    digitalWrite(enable3, HIGH);
    digitalWrite(enable4, HIGH); 
 
 
  digitalWrite(dir4, HIGH);

  analogWrite(pow4, 200);
 
}



