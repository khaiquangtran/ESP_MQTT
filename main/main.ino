
#include <ESP8266WiFi.h>  //library for using ESP8266 WiFi
#include <PubSubClient.h> //library for MQTT
#include <ArduinoJson.h>  //library for Parsing JSON

#define LED 2

uint32_t delayMS;
// MQTT Credentials
const char *ssid = "FPTSoftware HCM";                  // setting your ap ssid
const char *password = "4aesieunhan";          // setting your ap psk
const char *mqttServer = "broker.hivemq.com";   // MQTT URL
const char *mqttUserName = ""; // MQTT username
const char *mqttPwd = "";      // MQTT password
const char *topic = "Control/Home";             // publish topic
const char *topicSub = "Control/Receive";             // subcribe topic
// parameters for using non-blocking delay
unsigned long previousMillis = 0;
const long interval = 5000;

float temp, hum;
// setting up wifi and mqtt client
WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi()
{
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect()
{
  while (!client.connected())
  {
    String clientID =  "ESPClient-";
    clientID += String(random(0xffff),HEX);
    if (client.connect(clientID.c_str(), mqttUserName, mqttPwd))
    {
      Serial.println("MQTT connected");
      client.subscribe(topicSub);
      Serial.println("Topic Subscribed");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); // wait 5sec and retry
    }
  }
}
// subscribe call back
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String data = "";
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    data += (char)payload[i];
  }
  Serial.println();
  Serial.print("Message size :");
  Serial.println(length);
  Serial.println();
  Serial.println("-----------------------");
  Serial.println(data);

  if (data == "ON1")
  {
    digitalWrite(LED, HIGH);
    publicContent("ON1");
    return;
  }
  else if (data == "OFF1")
  {
    digitalWrite(LED, LOW);
    publicContent("OFF1");
    return;
  }
  else if (data == "ON2")
  {
    digitalWrite(LED, HIGH);
    publicContent("ON2");
    return;
  }
  else if (data == "OFF2")
  {
    digitalWrite(LED, LOW);
    publicContent("OFF2");
    return;
  }
  String substr = data.substring(0,2);
  Serial.println(substr);

  String hour11 = data.substring(data.indexof('/',0), data.indexof(':',0));
  String minute11 = data.substring(data.indexof(':',0));

  Serial.println("hour11 = " + hour11 );
  Serial.println("minute11 = " + minute11);
}
void setup()
{
  Serial.begin(115200);
  // Initialize device.

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  setup_wifi();
  client.setServer(mqttServer, 1883); // setting MQTT server
  client.setCallback(callback);       // defining function which will be called when message is received.
}
void loop()
{
  if (!client.connected())
  {              // if client is not connected
    reconnect(); // try to reconnect
  }
  client.loop();
  unsigned long currentMillis = millis(); // read current time
  if (currentMillis - previousMillis >= interval)
  { // if current time - last time > 5 sec
    previousMillis = currentMillis;
    // read temp and humidity

    
    delay(50);
  }
}

void publicContent(String content)
{
    String msgStr = content; // MQTT message buffer
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];
    Serial.print("PUBLISH DATA:");
    Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize);
    client.publish(topic, msg);
}
