#include <Wire.h>
#include <LPS.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <virtuabotixRTC.h> 

#ifndef STASSID
#define STASSID "Makinist"
#define STAPSK "tcdd2019"
#endif

virtuabotixRTC myRTC(2, 0, 16);


const char* server = "api.thingspeak.com";


File myFile;


String apiKey = "OVQON8DPHRXCNK6L"; 
const char* ssid = STASSID;
const char* password = STAPSK;

WiFiClient client;
LPS ps;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  //myRTC.setDS1302Time(0, 33, 9, 2, 14, 11, 2023);


  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(!SD.begin(15)) {
      Serial.println("initialization failed!");
      delay(500);

    }
    Serial.println("initialization done.");



  if (!ps.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");

  }

  ps.enableDefault();
}

void loop()
{
  float pressure = ps.readPressureMillibars();
  float altitude = ps.pressureToAltitudeMeters(pressure);
  float temperature = ps.readTemperatureC();
  myRTC.updateTime();
  Serial.print("GÜN / ZAMAN: ");                                                                 //| 
        Serial.print(myRTC.dayofmonth);                                                                        //| 
        Serial.print("/");                                                                                     //| 
        Serial.print(myRTC.month);                                                                             //| 
        Serial.print("/");                                                                                     //| 
        Serial.print(myRTC.year);                                                                              //| 
        Serial.print("  ");                                                                                    //| 
        Serial.print(myRTC.hours);                                                                             //| 
        Serial.print(":");                                                                                     //| 
        Serial.print(myRTC.minutes);                                                                           //| 
        Serial.print(":");                                                                                     //| 
        Serial.print(myRTC.seconds);  
        Serial.print(","); 
        Serial.print("Basınç ");
        Serial.print(pressure);
        Serial.print(" hPa\ta: ");
        Serial.print("Yükseklik: ");
        Serial.print(altitude);
        Serial.print(" m\tt: ");
        Serial.print("Sıcaklık: ");
        Serial.print(temperature);
        Serial.println(" deg C");



  myFile = SD.open("press.txt",FILE_WRITE);
  if (myFile)
   {    
        myFile.print("GÜN / ZAMAN: ");                                                                 //| 
        myFile.print(myRTC.dayofmonth);                                                                        //| 
        myFile.print("/");                                                                                     //| 
        myFile.print(myRTC.month);                                                                             //| 
        myFile.print("/");                                                                                     //| 
        myFile.print(myRTC.year);                                                                              //| 
        myFile.print("  ");                                                                                    //| 
        myFile.print(myRTC.hours);                                                                             //| 
        myFile.print(":");                                                                                     //| 
        myFile.print(myRTC.minutes);                                                                           //| 
        myFile.print(":");                                                                                     //| 
        myFile.print(myRTC.seconds);  
        myFile.print(",");     
        myFile.print("Basınç ");
        myFile.print(pressure);
        myFile.print(" hPa\ta: ");
        myFile.print("Yükseklik: ");
        myFile.print(altitude);
        myFile.print(" m\tt: ");
        myFile.print("Sıcaklık: ");
        myFile.print(temperature);
        myFile.println(" deg C");
        myFile.close();
        Serial.println("Sd karta yazma gerçekleştirildi");


   }
  if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(pressure);
                             postStr +="&field2=";
                             postStr += String(temperature);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);

                             Serial.print("Pressure: ");
                             Serial.print(pressure);
                             Serial.print(" hPa, Temperature: ");
                             Serial.print(temperature);
                             Serial.print(" °C ");
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
    
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);


}
