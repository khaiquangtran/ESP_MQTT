#ifndef _HEADER_H_
#define _HEADER_H_
#include <ESP8266WiFi.h>  //library for using ESP8266 WiFi
#include <PubSubClient.h> //library for MQTT
#include <ArduinoJson.h>  //library for Parsing JSON
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

#define LIGHT1 3 // fix for statable
#define LIGHT2 2 // fix for statable

uint32_t delayMS;
// MQTT Credentials
const char *ssid = "FPTSoftware HCM";         // setting your ap ssid
const char *password = "4aesieunhan";         // setting your ap psk
const char *mqttServer = "broker.hivemq.com"; // MQTT URL
const char *mqttUserName = "";                // MQTT username
const char *mqttPwd = "";                     // MQTT password
const char *topic = "Control/Home";           // publish topic
const char *topicSub = "Control/Receive";     // subcribe topic
// parameters for using non-blocking delay
unsigned long previousMillis = 0;
const long interval = 1000;

int hour11;
int minute11;
int hour12;
int minute12;
int hour21;
int minute21;
int hour22;
int minute22;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

String sYear;
String sMonth;
String sDay;
String sHour;
String sMinute;
String sSecond;

int second, minute, hour, day, wday, month, year;

// setting up wifi and mqtt client
WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

/* Address DS1307 */
const byte DS1307 = 0x68;
const byte NumberOfFields = 7;
const byte NumberOfTimeFields = 8;

bool mIsConnectMQTT = false;
bool mIsConnectWifi = false;

const byte OnOff1 = 0x08;
const byte OnOff2 = 0x0a;
const byte OnOff3 = 0x0c;
const byte OnOff4 = 0x0e;

bool flagSetTime = false;
int intervalreSetUpTime = 0;
const int milestonereSetTime = 3;
int previousDate = 0;

void OnLight1()
{
    digitalWrite(LIGHT1, HIGH);
}

void OffLight1()
{
    digitalWrite(LIGHT1, LOW);
}

void OnLight2()
{
    digitalWrite(LIGHT2, HIGH);
}

void OffLight2()
{
    digitalWrite(LIGHT2, LOW);
}

bool setup_wifi()
{
    int timer = 0;
    delay(10);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        timer++;
        if (timer >= 20)
        {
            Serial.println("Can not connect wifi");
            return false;
        }
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

bool reconnect()
{
    int timer = 0;
    while (!client.connected())
    {
        String clientID = "ESPClient-";
        clientID += String(random(0xffff), HEX);
        if (client.connect(clientID.c_str(), mqttUserName, mqttPwd))
        {
            Serial.println("MQTT connected");
            client.subscribe(topicSub);
            Serial.println("Topic Subscribed");
            return true;
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000); // wait 5sec and retry
        }
        timer++;
        if (timer >= 4)
        {
            return false;
        }
    }
    return true;
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

int bcd2dec(byte num)
{
    return ((num / 16 * 10) + (num % 16));
}

int dec2bcd(byte num)
{
    return ((num / 10 * 16) + (num % 10));
}

void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr)
{
    Wire.beginTransmission(DS1307);
    Wire.write(byte(0x00)); // đặt lại pointer
    Wire.write(dec2bcd(sec));
    Wire.write(dec2bcd(min));
    Wire.write(dec2bcd(hr));
    Wire.write(dec2bcd(wd)); // day of week: Sunday = 1, Saturday = 7
    Wire.write(dec2bcd(d));
    Wire.write(dec2bcd(mth));
    Wire.write(dec2bcd(yr));
    Wire.endTransmission();
}

void setTimeOnOff(byte ptr, byte hour, byte min)
{
    Wire.beginTransmission(DS1307);
    Wire.write(byte(ptr)); // đặt lại pointer
    Wire.write(dec2bcd(hour));
    Wire.write(dec2bcd(min));
    Wire.endTransmission();
}

void readTimeSetUp()
{
    Wire.beginTransmission(DS1307);
    Wire.write((byte)0x08);
    Wire.endTransmission();
    Wire.requestFrom(DS1307, NumberOfTimeFields);

    minute11 = bcd2dec(Wire.read());
    hour11 = bcd2dec(Wire.read());
    minute12 = bcd2dec(Wire.read());
    hour12 = bcd2dec(Wire.read());
    minute21 = bcd2dec(Wire.read());
    hour21 = bcd2dec(Wire.read());
    minute22 = bcd2dec(Wire.read());
    hour22 = bcd2dec(Wire.read());
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
        OnLight1();
        publicContent("ON1");
        return;
    }
    else if (data == "OFF1")
    {
        OffLight1();
        publicContent("OFF1");
        return;
    }
    else if (data == "ON2")
    {
        OnLight2();
        publicContent("ON2");
        return;
    }
    else if (data == "OFF2")
    {
        OffLight2();
        publicContent("OFF2");
        return;
    }
    else if (data == "ALL")
    {
        String msgStr;
        String light1;
        String light2;

        if (digitalRead(LIGHT1))
        {
            light1 = "ON1";
        }
        else
        {
            light1 = "OFF1";
        }
        if (digitalRead(LIGHT2))
        {
            light2 = "ON2";
        }
        else
        {
            light2 = "OFF2";
        }
        readTimeSetUp();

        msgStr = "ALL/" + light1 + "/" + light2 + "/" + String(hour11) + ":" + String(minute11) + "/" + String(hour12) + ":" + String(minute12) + "/" + String(hour21) + ":" + String(minute21) + "/" + String(hour22) + ":" + String(minute22);

        byte arrSize = msgStr.length() + 1;
        char msg[arrSize];
        Serial.print("PUBLISH DATA:");
        Serial.println(msgStr);
        msgStr.toCharArray(msg, arrSize);
        client.publish(topic, msg);
    }

    String substr = data.substring(0, 2);
    String hour = data.substring(data.indexOf('/', 0) + 1, data.indexOf(':', 0));
    String minute = data.substring(data.indexOf(':', 0) + 1);

    if (substr == "11")
    {
        hour11 = hour.toInt();
        minute11 = minute.toInt();
        setTimeOnOff(OnOff1, hour11, minute11);
        publicContent("11");
        return;
    }
    if (substr == "12")
    {
        hour12 = hour.toInt();
        minute12 = minute.toInt();
        setTimeOnOff(OnOff2, hour12, minute12);
        publicContent("12");
        return;
    }
    if (substr == "21")
    {
        hour21 = hour.toInt();
        minute21 = minute.toInt();
        setTimeOnOff(OnOff3, hour21, minute21);
        publicContent("21");
        return;
    }
    if (substr == "22")
    {
        hour22 = hour.toInt();
        minute22 = minute.toInt();
        setTimeOnOff(OnOff4, hour22, minute22);
        publicContent("22");
        return;
    }

    Serial.println(substr);
    Serial.println("hour = " + hour);
    Serial.println("minute = " + minute);
}

void setUpRTC()
{
    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    // We need to extract date and time
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();
    Serial.println(formattedDate);

    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    sYear = dayStamp.substring(0, formattedDate.indexOf("-"));
    sMonth = dayStamp.substring(formattedDate.indexOf("-") + 1, formattedDate.indexOf("-") + 3);
    sDay = dayStamp.substring(dayStamp.indexOf(sMonth) + 1, dayStamp.indexOf(sMonth) + 3);
    Serial.print("DATE: ");
    Serial.println(dayStamp);
    Serial.print("Year: ");
    Serial.println(sYear);
    Serial.print("Month: ");
    Serial.println(sMonth);
    Serial.print("Day: ");
    Serial.println(sDay);

    // Extract time
    timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
    sHour = timeStamp.substring(0, timeStamp.indexOf(":"));
    sMinute = timeStamp.substring(timeStamp.indexOf(sHour) + 3, timeStamp.indexOf(sHour) + 5);
    sSecond = timeStamp.substring(timeStamp.indexOf(sMinute) + 3, timeStamp.indexOf(sMinute) + 5);
    Serial.print("HOUR: ");
    Serial.println(timeStamp);
    Serial.print("Hour: ");
    Serial.println(sHour);
    Serial.print("Minute: ");
    Serial.println(sMinute);
    Serial.print("Second: ");
    Serial.println(sSecond);

    year = sYear.toInt();
    month = sMonth.toInt();
    day = sDay.toInt();
    hour = sHour.toInt();
    minute = sMinute.toInt();
    second = sSecond.toInt();
    setTime(hour, minute, second, 1, day, month, (int)(year % 100)); // 12:30:45 CN 08-02-2015
}

void readDS1307()
{
    Wire.beginTransmission(DS1307);
    Wire.write((byte)0x00);
    Wire.endTransmission();
    Wire.requestFrom(DS1307, NumberOfFields);

    second = bcd2dec(Wire.read() & 0x7f);
    minute = bcd2dec(Wire.read());
    hour = bcd2dec(Wire.read() & 0x3f); // chế độ 24h.
    wday = bcd2dec(Wire.read());
    day = bcd2dec(Wire.read());
    month = bcd2dec(Wire.read());
    year = bcd2dec(Wire.read());
    year += 2000;
}

void OnOffLight()
{
    if(hour == hour11 && minute == minute11 && (second == 1 || second == 2))
    {
        OnLight1();
    }
    if(hour == hour12 && minute == minute12 && (second == 1 || second == 2))
    {
        OffLight1();
    }
    if(hour == hour21 && minute == minute21 && (second == 1 || second == 2))
    {
        OnLight2();
    }
    if(hour == hour22 && minute == minute22 && (second == 1 || second == 2))
    {
        OffLight1();
    }
}

void reSetUpTimeWhenConnectWifi()
{
    if(hour == 1 && minute == 1 && (second == 1 || second == 2) && flagSetTime == false)
    {
        setUpRTC();
        flagSetTime = true;
    }
    if(hour == 1 && minute == 2 && (second == 1 || second == 2))
    {
        flagSetTime = false;
    }
}

#endif _HEADER_H_
