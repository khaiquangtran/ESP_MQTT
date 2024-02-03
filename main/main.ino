#include "header.h"

void setup()
{
  Serial.begin(115200);
  // Initialize device.

  pinMode(LIGHT1, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  digitalWrite(LIGHT1, LOW);
  digitalWrite(LIGHT2, LOW);

  Wire.begin();
  mIsConnectWifi = setup_wifi();
  client.setServer(mqttServer, 1883); // setting MQTT server
  mIsConnectMQTT = reconnect();
  
  if (mIsConnectWifi && mIsConnectMQTT)
  {
    client.setCallback(callback); // defining function which will be called when message is received.
    setUpRTC();
  }
  else
  {
    readDS1307();
    intervalreSetUpTime = 0;
  }
  readTimeSetUp();
  previousDate = day;
}

void loop()
{
  if (mIsConnectWifi && mIsConnectMQTT)
  {
    client.loop();
  }

  unsigned long currentMillis = millis(); // read current time
  if (currentMillis - previousMillis >= interval)
  { // if current time - last time > 1 sec
    previousMillis = currentMillis;
    readDS1307();
    OnOffLight();
    if (mIsConnectWifi)
    {
      reSetUpTimeWhenConnectWifi();
    }
    else
    {
      if (previousDate > day || previousDate < day)
      {
        intervalreSetUpTime++;
      }
      if(hour == 10 && minute == 1 && ( second == 30 || second == 29) && intervalreSetUpTime == milestonereSetTime)
      {
        setTime(hour, minute, 25, 1, day, month, (int)(year % 100)); // 12:30:45 CN 08-02-2015
        intervalreSetUpTime = 0;
      }
    }
  }

  delay(100);
}
