


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


// MiniMaestroServices construction
SoftwareSerial maestroSerial(22, 23); // Connect A1 to Maestro's RX. A0 must remain disconnected.
MiniMaestroService maestro(maestroSerial);

// Subsystems construction
HS485 chamber = HS485(maestro, 0); // Some objects can use MiniMaestroService to controll devices. See docs.
HS485 intake = HS485(maestro, 1);
TalonSR shooter = TalonSR(maestro, 2);
PololuG2 intakemotor = PololuG2(maestro, 3, 4, 5);

// Drive base construction
PololuG2 rightmotor1 = PololuG2(2, 3, 4);
PololuG2 rightmotor2 = PololuG2(5, 6, 7);
PololuG2 leftmotor1 = PololuG2(8, 9, 10);
PololuG2 leftmotor2 = PololuG2(13, 11, 12);

// Can construct drive bases using any speed controllers extended from Motor class. (TalonSR and PololuG2).
TankDrive chassis = TankDrive(leftmotor1, leftmotor2, rightmotor1, rightmotor2);

// Network & Controller construction
PS2X ps2x;
NetworkTable network = NetworkTable(10, 10);
PacketSerial myPacketSerial;

float leftvalue = 0;
float rightvalue = 0;
unsigned long last_blink;
unsigned long last_update;
void setup() {
  // prevents devices from actuating on startup.
  delay(1000);

  // initialize MiniMaestroService communication
  maestroSerial.begin(115200);

  // enable ps2x networking
  ps2x.config_gamepad();
  ps2x.read_gamepad();

  // enable debug usb (Serial) and radio serial (Serial1)
  Serial.begin(115200);
  Serial1.begin(115200);

  // binds radio PacketSerial(encoding&decoding services) to NetworkTable class. Uses lambda expressions. 
  myPacketSerial.setStream(&Serial1);
  myPacketSerial.setPacketHandler([](const uint8_t* buffer, size_t size) {
  network.processPacketFromSender(myPacketSerial, buffer, size);
  });

  // sets ps2x controlls.
  network.setPS2(ps2x);

  // reverses forward direction of left tankdrive wheels.
  chassis.reverseLeftMotors(true);
}

void ledService()
{
  // cant add any led services right now as pin 13 is being used for driving a pololuG2.
}

void loop() {
  // trigger network services
  myPacketSerial.update();

  // trigger led (human readable) services.
  ledService();
  
  // drive chassis
  chassis.drive(ps2x);

  // update target values every 9ms
  if(millis() - last_update > 9)
  {
    // Button Variables
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
    // actuate devices to intake tennis balls. Arguments are experimetnally determined / calculated.
    if(R1_Pressed)
    {
      intakemotor.setPower(-1); // all speed controllers extended from Motor class have setPower(float power) function. Value between -1 (FUll Reverse) to 1 (FULL Forward).
      chamber.setPosition(170); // HS485 servos have setPosition(float position). Value between 0 and 180 degrees.
      intake.setPosition(90.7);
    }

    // return to idle positions
    if(R1_Released)
    {
      intakemotor.setPower(0);
      chamber.setPosition(128);
      intake.setPosition(178);
    }
    
    // Outake
    if(L1_Pressed)
    {
      intakemotor.setPower(1);
      chamber.setPosition(170);
      intake.setPosition(90.7);
    }

    if(L1_Released)
    {
      intakemotor.setPower(0);
      chamber.setPosition(128);
      intake.setPosition(178);
    }
    
    // Actuate Chamber (put tennis ball into shooter)
    if(R2_Pressed)
    {
      chamber.setPosition(30);
    }

    if(R2_Released)
    {
      chamber.setPosition(128);
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

  // Trigger MiniMaestroService
  maestro.service();
}

