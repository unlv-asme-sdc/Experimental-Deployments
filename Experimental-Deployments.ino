


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


// MaestroServices
SoftwareSerial maestroSerial(22, 23); // Connect A1 to Maestro's RX. A0 must remain disconnected.
MiniMaestroService maestro(maestroSerial);

// Subsystems
HS485 chamber = HS485(maestro, 0);
HS485 intake = HS485(maestro, 1);
TalonSR shooter = TalonSR(maestro, 2);
PololuG2 intakemotor = PololuG2(maestro, 3, 4, 5);

// Drive base
PololuG2 rightmotor1 = PololuG2(2, 3, 4);
PololuG2 rightmotor2 = PololuG2(5, 6, 7);
PololuG2 leftmotor1 = PololuG2(8, 9, 10);
PololuG2 leftmotor2 = PololuG2(13, 11, 12);

TankDrive chassis = TankDrive(leftmotor1, leftmotor2, rightmotor1, rightmotor2);

// Networking
PS2X ps2x;
NetworkTable network = NetworkTable(10, 10);
PacketSerial myPacketSerial;

float leftvalue = 0;
float rightvalue = 0;
unsigned long last_blink;
unsigned long last_update;
void setup() {
  maestroSerial.begin(115200);
  ps2x.config_gamepad();
  ps2x.read_gamepad();
  Serial.begin(115200);
  Serial1.begin(115200);
  myPacketSerial.setStream(&Serial1);
  myPacketSerial.setPacketHandler([](const uint8_t* buffer, size_t size) {
  network.processPacketFromSender(myPacketSerial, buffer, size);
  });
  network.setPS2(ps2x);
  chassis.reverseLeftMotors(true);
}

void ledService()
{
  unsigned long value = millis() - last_blink;
  if(value > 500)
  {
    last_blink = millis();
  }
}

void loop() {
  
  myPacketSerial.update();
  ledService();
  chassis.drive(ps2x);
  
  if(millis() - last_update > 20)
  {
    bool cross = ps2x.Button(PSB_CROSS);
    bool circle = ps2x.Button(PSB_CIRCLE);
    bool triangle = ps2x.Button(PSB_TRIANGLE);
    bool square = ps2x.Button(PSB_SQUARE);
    bool L1 = ps2x.Button(PSB_L1);
    bool L2 = ps2x.Button(PSB_L2);
    bool R1 = ps2x.Button(PSB_R1);
    bool R2 = ps2x.Button(PSB_R2);
    bool R1_Pressed = ps2x.ButtonPressed(PSB_R1);
    bool R1_Released = ps2x.ButtonReleased(PSB_R1);
    bool L1_Pressed = ps2x.ButtonPressed(PSB_L1);
    bool L1_Released = ps2x.ButtonReleased(PSB_L1);
    bool R2_Pressed = ps2x.ButtonPressed(PSB_R2);
    bool R2_Released = ps2x.ButtonReleased(PSB_R2);
    bool L2_Pressed = ps2x.ButtonPressed(PSB_L2);
    bool L2_Released = ps2x.ButtonReleased(PSB_L2);

    // Intake
    if(R1_Pressed)
    {
      intakemotor.setPower(-0.5);
      chamber.setPosition(135);
      intake.setPosition(65);
    }

    if(R1_Released)
    {
      intakemotor.setPower(0);
      chamber.setPosition(93);
      intake.setPosition(157);
    }
    
    // Outake
    if(L1_Pressed)
    {
      intakemotor.setPower(1);
      chamber.setPosition(150);
      intake.setPosition(65);
    }

    if(L1_Released)
    {
      intakemotor.setPower(0);
      chamber.setPosition(93);
      intake.setPosition(157);
    }
    
    // Actuate Chamber (put tennis ball into shooter)
    if(R2_Pressed)
    {
      chamber.setPosition(30);
    }

    if(R2_Released)
    {
      chamber.setPosition(93);
    }
    
    //Arm Shooter
    if(L2)
    {
      shooter.setPower(-1);
    } else {
      shooter.setPower(0);
    }
 
    last_update = millis();
  }

  if(network.getLastPS2PacketTime() > 500)
  {
    digitalWrite(2, LOW);
    digitalWrite(5, LOW);
    digitalWrite(8, LOW);
    digitalWrite(13, LOW);
    maestro.queTarget(3, LOW);
    shooter.setPower(0);
  } else {
    digitalWrite(2, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(13, HIGH);
    maestro.queTarget(3, HIGH);

  }
  
  maestro.service();
}

