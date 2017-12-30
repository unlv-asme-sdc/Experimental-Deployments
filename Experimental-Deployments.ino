


#include <SoftwareSerial.h>
#include <MiniMaestroService.h>
#include <TalonSR.h>
#include <PololuG2.h>
#include <TankDrive.h>
#include <PS2X_lib.h>
#include <HS485.h>
#include <NetworkTable.h>
/*
SoftwareSerial maestroSerial(10, 11);
MiniMaestro maestro(maestroSerial);
TalonSR talon = TalonSR(maestro, 1);
*/

PololuG2 rightmotor1 = PololuG2(2, 3, 4);
PololuG2 rightmotor2 = PololuG2(5, 6, 7);
PololuG2 leftmotor1 = PololuG2(8, 9, 10);
PololuG2 leftmotor2 = PololuG2(13, 11, 12);
//SoftwareSerial maestroSerial(18, 19); // Connect A1 to Maestro's RX. A0 must remain disconnected.
//iniMaestroService maestro(maestroSerial);
//HS485 chamber = HS485(maestro, 0);
//HS485 intake = HS485(maestro, 1);
//TalonSR shooter = TalonSR(maestro, 2);
TankDrive chassis = TankDrive(&leftmotor1, &leftmotor2, &rightmotor1, &rightmotor2);
PS2X ps2x;
NetworkTable network = NetworkTable(10, 10);
PacketSerial myPacketSerial;

float leftvalue = 0;
float rightvalue = 0;
unsigned long last_blink;
unsigned long last_update;
void setup() {
  //maestroSerial.begin(115200);
  ps2x.config_gamepad();
  ps2x.read_gamepad();
  Serial.begin(115200);
  Serial1.begin(115200);
  myPacketSerial.setStream(&Serial1);
  myPacketSerial.setPacketHandler([](const uint8_t* buffer, size_t size) {
  network.processPacketFromSender(myPacketSerial, buffer, size);
  });
  network.setPS2(&ps2x);
  chassis.reverseLeftMotors(true);
}

void ledService()
{
  unsigned long value = millis() - last_blink;
  if(value > 500)
  {
    last_blink = millis();
    Serial.println("working");
  }
}

void loop() {
  
  myPacketSerial.update();
  ledService();
  chassis.drive(ps2x);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  
  if(network.getLastPS2PacketTime() > 500)
  {
    digitalWrite(2, LOW);
    digitalWrite(5, LOW);
    digitalWrite(8, LOW);
    digitalWrite(13, LOW);
  } else {
    digitalWrite(2, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(13, HIGH);
  }
  /*
  if(millis() - last_update > 20)
  {
    int cross = ps2x.Button(PSB_CROSS);
    int circle = ps2x.Button(PSB_CIRCLE);
    int triangle = ps2x.Button(PSB_TRIANGLE);
    int square = ps2x.Button(PSB_SQUARE);
    
    /*
    if(cross)
    {
      shooter.setPower(-1);
    }
    if(circle)
    {
      shooter.setPower(0); 
    }
    if(triangle)
    {
      chamber.setPosition(135);
    }
    if(square)
    {
      chamber.setPosition(30);
    }
    last_update = millis();
  }
  */
  //maestro.service();
}

