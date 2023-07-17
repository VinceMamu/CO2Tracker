#include <SD.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <virtuabotixRTC.h>

File myFile;
int RXPin = 7;
int TXPin = 6;

TinyGPSPlus gps;
SoftwareSerial SerialGPS(RXPin, TXPin);
virtuabotixRTC myRTC(3, 4, 5);

String Latitude, Longitude, Date, Time, Data;

char sendLatitude[8];
char sendLongitude[8];
char sendDate[11];
char sendTime[9];

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  Serial.println("Creating GPS_data.csv...");
  myFile = SD.open("GPS_data.csv", FILE_WRITE);
  if (myFile) {
    myFile.println("Latitude, Longitude, Date, Time\r\n");
    myFile.close();
  } else {
    Serial.println("error opening GPS_data.csv");
  }
  SerialGPS.begin(9600);
}

void loop() {
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
      obtain_data();
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("GPS NOT DETECTED!");
    while (true);
  }
  obtainRTC();
  stringToCharArray(Latitude, sendLatitude, 8);
  stringToCharArray(Longitude, sendLongitude, 8);
  stringToCharArray(Time, sendTime, 9);
  stringToCharArray(Date, sendDate, 11);

  Serial.write(sendLatitude, 8); //Write the serial data
  Serial.write(sendLongitude, 8);
  Serial.write(sendTime, 9);
  Serial.write(sendDate, 11);
  delay(5000);
}

void obtain_data() {
  long startTime = millis();
  while(millis() - startTime < 5000){
    if(gps.location.isValid()){
      float lat = gps.location.lat();
      float lng = gps.location.lng();
    } else{
      Latitude = "No Data";
      Longitude = "No Data";
      Serial.println("Location is not available");
      }
  }

  Data = Latitude + "," + Longitude + "," + Date + "," + Time;
  Serial.print("Save data: ");
  Serial.println(Data);
  myFile = SD.open("GPS_data.csv", FILE_WRITE);

  if (myFile) {
    Serial.print("GPS logging to GPS_data.csv...");
    myFile.println(Data);
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening GPS_data.csv");
  }
  Serial.println();
}

void stringToCharArray(const String& str, char* charArray, size_t bufferSize) {
  str.toCharArray(charArray, bufferSize);
}

void obtainRTC() {
  // Update the time
  myRTC.updateTime();

  // Print the current date and time
  Serial.print(myRTC.dayofmonth);
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print(" ");

  Date = String(myRTC.month) + "/" + String(myRTC.dayofmonth) + "/" + String(myRTC.year);

  // Convert 24-hour format to 12-hour format and display with AM/PM indication
  if (myRTC.hours == 0) {
    Time = "12:" + String(myRTC.minutes) + ":" + String(myRTC.seconds) + " AM";
    Serial.print("12");
  } else if (myRTC.hours > 12) {
    int hourValue = myRTC.hours - 12;
    Time = String(hourValue) + ":";
    if (myRTC.minutes < 10) {
      Time += "0";
    }
    Time += String(myRTC.minutes) + ":" + String(myRTC.seconds) + " PM";
    Serial.print(hourValue);
  } else {
    Time = String(myRTC.hours) + ":";
    if (myRTC.minutes < 10) {
      Time += "0";
    }
    Time += String(myRTC.minutes) + ":" + String(myRTC.seconds) + " AM";
    Serial.print(myRTC.hours);
  }

  Serial.print(":");
  if (myRTC.minutes < 10) {
    Serial.print("0");
  }
  Serial.print(myRTC.minutes);
  Serial.print(":");
  if (myRTC.seconds < 10) {
    Serial.print("0");
  }
  Serial.print(myRTC.seconds);

  if (myRTC.hours < 12) {
    Serial.println(" AM");
  } else {
    Serial.println(" PM");
  }
}
