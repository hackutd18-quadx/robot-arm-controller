// Installed libraries
#include <ArduinoHttpClient.h>

// Arm functions include
#include "arm.h"

// Standard library includes
#include <SPI.h>
#include <Ethernet.h>

// Define enable pin
#define EN 5

// Define direction pins
#define ARM_DIR 3
#define EXTENSION_DIR 6
#define CLAW_DIR 8

// Define step pins
#define ARM_STEP 4
#define EXTENSION_STEP 7
#define CLAW_STEP 9

// Global ethernet variables
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
char server[] = "echo.websocket.org";
int port = 80;

// Global position variables
int armSteps, extensionSteps, clawSteps = 0;

double currentArmAngle = 0;
double currentExtensionAngle = 180;
double currentClawAngle = 0;

double targetArmAngle = 90;
double targetExtensionAngle = currentExtensionAngle + (targetArmAngle / ARM_GEAR_RATIO);
double targetClawAngle = 0;

double posX, posY = 0;
double lastX, lastY = 0;

// Websocket client
EthernetClient ethernet;
WebSocketClient client = WebSocketClient(ethernet, server, port);

void readSocket()
{
  if (client.connected())
  {
    // Check if a message is available to be received
    int messageSize = client.parseMessage();

    if (messageSize > 0)
    {
      Serial.println("Received a message:");
      Serial.println(client.readString());

      // Parse the message to set the new x and y positions
    }
  }
}

void stepArm()
{
  const double diff = fabs(targetArmAngle - currentArmAngle);
  if (diff > 0.1)
  {
    if (targetArmAngle > currentArmAngle)
    {
      step(false, ARM_DIR, ARM_STEP, 100);
      currentArmAngle += stepsToDegrees(1) / ARM_GEAR_RATIO;
    }
    else if (targetArmAngle < currentArmAngle)
    {
      step(true, ARM_DIR, ARM_STEP, 100);
      currentArmAngle -= stepsToDegrees(1) / ARM_GEAR_RATIO;
    }
  }
  //Serial.print("arm angle: "); Serial.println(currentArmAngle);
  /*
  if (armSteps < degreesToSteps(targetArmAngle) * ARM_GEAR_RATIO)
  {
    boolean dir = checkAngle(currentArmAngle, targetArmAngle);
    step(dir, ARM_DIR, ARM_STEP, 100);
    armSteps++;
    if (dir)
    {
      currentArmAngle += stepsToDegrees(1) * ARM_GEAR_RATIO;
    }
    else
    {
      currentArmAngle -= stepsToDegrees(1) * ARM_GEAR_RATIO;
    }
  }
  */
  // else {
  //   Serial.print("arm angle: "); Serial.println(currentArmAngle);
  // }
}

void stepExtension()
{
  const double diff = fabs(targetExtensionAngle - currentExtensionAngle);
  if (diff > 0.1)
  {
    if (targetExtensionAngle > currentExtensionAngle)
    {
      step(false, EXTENSION_DIR, EXTENSION_STEP, 100);
      currentExtensionAngle += stepsToDegrees(1) / EXTENSION_GEAR_RATIO;
    }
    else if (targetExtensionAngle < currentExtensionAngle)
    {
      step(true, EXTENSION_DIR, EXTENSION_STEP, 100);
      currentExtensionAngle -= stepsToDegrees(1) / EXTENSION_GEAR_RATIO;
    }
  }
  //Serial.print("extension angle: "); Serial.println(currentExtensionAngle);
  /*
  if (extensionSteps < degreesToSteps(targetExtensionAngle) * EXTENSION_GEAR_RATIO)
  {
    boolean dir = checkAngle(currentExtensionAngle, targetExtensionAngle);
    //Serial.print("Steps: "); Serial.println(extensionSteps);
    step(dir, EXTENSION_DIR, EXTENSION_STEP, 100);
    //Serial.print("Target: "); Serial.println(degreesToSteps(targetExtensionAngle));
    extensionSteps++;
    if (dir)
    {
      currentExtensionAngle += stepsToDegrees(1) * EXTENSION_GEAR_RATIO;
    }
    else
    {
      currentExtensionAngle -= stepsToDegrees(1) * EXTENSION_GEAR_RATIO;
    }
  }
  */
  // else {
  //   Serial.print("extension angle: "); Serial.println(currentExtensionAngle);
  // }
}

void stepClaw()
{
  if (clawSteps < degreesToSteps(targetClawAngle))
  {
    boolean dir = checkAngle(currentClawAngle, targetClawAngle);
    step(dir, CLAW_DIR, CLAW_STEP, 100);
    clawSteps++;
  }
}

/**
 * return true: rotate cw, return false: rotate ccw
 */
boolean checkAngle(double current, double target)
{
  if (current < target)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void setup()
{
  Serial.begin(9600);

  Serial.print("arm angle: ");
  Serial.println(targetArmAngle);
  Serial.print("extension angle: ");
  Serial.println(targetExtensionAngle);

  // Initialize ip connection
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP!");
  }
  else
  {
    Serial.println(Ethernet.localIP());
  }

  pinMode(ARM_DIR, OUTPUT);
  pinMode(ARM_STEP, OUTPUT);
  pinMode(EXTENSION_DIR, OUTPUT);
  pinMode(EXTENSION_STEP, OUTPUT);
  pinMode(CLAW_DIR, OUTPUT);
  pinMode(CLAW_STEP, OUTPUT);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, HIGH);

  // Start websocket client
  // client.begin();
}
int count = 0;
void loop()
{
  //readSocket();
  stepArm();
  stepExtension();
  stepClaw();

  if (count >= 1000)
  {
    delay(500);
    Serial.print("arm target: ");
    Serial.println(targetArmAngle);
    Serial.print("extension target: ");
    Serial.println(targetExtensionAngle);
    Serial.print("arm current: ");
    Serial.println(currentArmAngle);
    Serial.print("extension current: ");
    Serial.println(currentExtensionAngle);
    delay(500);
    count = 0;
  }

  count++;

  lastX = posX;
  lastY = posY;
}
