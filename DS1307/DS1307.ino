#include <Wire.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

/* Địa chỉ của DS1307 */
const byte DS1307 = 0x68;
/* Số byte dữ liệu sẽ đọc từ DS1307 */
const byte NumberOfFields = 7;

/* khai báo các biến thời gian */
int second, minute, hour, day, wday, month, year;

// Replace with your network credentials
const char* ssid     = "FPTSoftware HCM";
const char* password = "4aesieunhan";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

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

void setup()
{
  //  Wire.begin();
  /* cài đặt thời gian cho module */
    setTime(12, 30, 45, 1, 8, 2, 15); // 12:30:45 CN 08-02-2015
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop()
{
//  while (!timeClient.update()) {
//    timeClient.forceUpdate();
//  }
//  // The formattedDate comes with the following format:
//  // 2018-05-28T16:00:13Z
//  // We need to extract date and time
//  formattedDate = timeClient.getFormattedDate();
//  Serial.println(formattedDate);
//
//  // Extract date
//  int splitT = formattedDate.indexOf("T");
//  dayStamp = formattedDate.substring(0, splitT);
//  sYear = dayStamp.substring(0, formattedDate.indexOf("-"));
//  sMonth = dayStamp.substring(formattedDate.indexOf("-") + 1, formattedDate.indexOf("-") + 3);
//  sDay = dayStamp.substring(dayStamp.indexOf(sMonth) + 1, dayStamp.indexOf(sMonth) + 3);
//  Serial.print("DATE: ");
//  Serial.println(dayStamp);
//  Serial.print("Year: ");
//  Serial.println(sYear);
//  Serial.print("Month: ");
//  Serial.println(sMonth);
//  Serial.print("Day: ");
//  Serial.println(sDay);
//
//  // Extract time
//  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
//  sHour = timeStamp.substring(0, timeStamp.indexOf(":"));
//  sMinute = timeStamp.substring(timeStamp.indexOf(sHour) + 3, timeStamp.indexOf(sHour) + 5);
//  sSecond = timeStamp.substring(timeStamp.indexOf(sMinute) + 3, timeStamp.indexOf(sMinute) + 5);
//  Serial.print("HOUR: ");
//  Serial.println(timeStamp);
//  Serial.print("Hour: ");
//  Serial.println(sHour);
//  Serial.print("Minute: ");
//  Serial.println(sMinute);
//  Serial.print("Second: ");
//  Serial.println(sSecond);
  delay(1000);

    /* Đọc dữ liệu của DS1307 */
    readDS1307();
    /* Hiển thị thời gian ra Serial monitor */
    digitalClockDisplay();
//    delay(1000);
}

void readDS1307()
{
  Wire.beginTransmission(DS1307);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307, NumberOfFields);

  second = bcd2dec(Wire.read() & 0x7f);
  minute = bcd2dec(Wire.read() );
  hour   = bcd2dec(Wire.read() & 0x3f); // chế độ 24h.
//  wday   = bcd2dec(Wire.read() );
  day    = bcd2dec(Wire.read() );
  month  = bcd2dec(Wire.read() );
  year   = bcd2dec(Wire.read() );
  year += 2000;
}
/* Chuyển từ format BCD (Binary-Coded Decimal) sang Decimal */
int bcd2dec(byte num)
{
  return ((num / 16 * 10) + (num % 16));
}
/* Chuyển từ Decimal sang BCD */
int dec2bcd(byte num)
{
  return ((num / 10 * 16) + (num % 10));
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour);
//  printDigits(minute);
//  printDigits(second);
  Serial.print(" ");
  Serial.print(day);
  Serial.print(" ");
  Serial.print(month);
  Serial.print(" ");
  Serial.print(year);
  Serial.println();
}

void printDigits(int digits) {
  // các thành phần thời gian được ngăn chách bằng dấu :
  Serial.print(":");

  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/* cài đặt thời gian cho DS1307 */
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
