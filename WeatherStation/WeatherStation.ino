#include <ESP8266WiFi.h>
#include <WString.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  

#include <math.h>

///////////////////////////
#include "Read_DHT22.h"
#include "Serlcd.h"
#include "ds1307.h"
#include "MYBMP180.h"
///////////////////////////

int button = 2;

int flag=0; //button flag 

char *tem="";
/*Use your own apikey and deviceID to replace the following variables*/
String apikey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char* deviceID="xxxxxxxxxxx";

const char* server = "www.linksprite.io";
WiFiClient client;
char * float2str(float val, int precision, char *buf);

float sensorVoltage; 

void display_logo()
{
    lcdPosition(0,0);
    LCD.print("   LinkSprite   ");
    lcdPosition(1,0);
    LCD.print("Weather Station");
}

/*Read the RTC device and display the time on LCD screen*/
void display_RTC()
{
  lcdPosition(0,0);
  getDateDs1307();
  if(hour<10) LCD.print('0');
  LCD.print(hour,DEC);
  LCD.print(":");
  if(minute<10) LCD.print('0');
  LCD.print(minute,DEC);
  LCD.print(":");
  if(second<10) LCD.print('0');
  LCD.print(second, DEC);
  LCD.print("  ");

  if(month<10) LCD.print('0');
  LCD.print(month, DEC);
  LCD.print("/");
  if(dayOfMonth<10) LCD.print('0');
  LCD.print(dayOfMonth, DEC);
  LCD.print(" ");
  //LCD.print(year, 2);
  //LCD.println(" ");
}

/*Read the DHT22 sensor and display temperature&humidity data on LCD screen*/
void display_dht22()
{
  static int i=0;
  i++;
  if(i==20)
  {
    i=0;
    read_dht22_data();
    lcdPosition(1,0);
    LCD.print(t,1);
    LCD.print("C ");
   
    lcdPosition(1,8);
    LCD.print(h,1);
    LCD.print("%");
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.println("\n-----------------------------------");
}

/*Read the atmospheric pressure and display it on LCD screen*/
void display_bmp180()
{
   get_bmp180();
   delay(100);
   lcdPosition(0,0);
   LCD.print("Press:");
   LCD.print(pressure,0);
   LCD.print("Pa");

   lcdPosition(1,0);
   LCD.print("altitude:");
   LCD.print(altitude,1);
   LCD.print("m");
}

/*Using WiFiManager to config and connect a wireless router*/
void linksprite_io_init()
{
   WiFiManager wifiManager;
   wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
   wifiManager.autoConnect("LinkNodeAP");
   Serial.print("WiFi Connected ...\n");
   Serial.println("WiFi connected");
}

/*Upload the sensor's data to linksprite.io*/
void UpdateToLinkSpriteIO()
{
  String request1 = "";
  String request = "";
  //clearLCD(); 
  //lcdPosition(0,0);
  //LCD.print("Update To LinkSp");
  //LCD.print("riteIO Uploading");
  if (client.connect(server,80)) 
  {  
     String  postStr ="{";
           postStr +="\"action\":\"update\",";
           postStr +="\"apikey\":\"";
           postStr += apikey;
           postStr +="\",";
           postStr +="\"deviceid\":\"";
           postStr += deviceID;
           postStr +="\",";
           postStr +="\"params\":";
           postStr +="{";
           
           postStr +="\"pressure\":\"";
           itoa(pressure,tem,10); 
           postStr +=tem;
           postStr +="\",";
           
           postStr +="\"temperature\":\"";
           itoa(t,tem,10); 
           postStr +=tem;
           postStr +="\",";

           //postStr +="\"door\":\"";
           //itoa(door_state,tem,10); 
           //postStr +=tem;
           //postStr +="\",";
           
           postStr +="\"humidity\":\"";
           itoa(h,tem,10); 
           postStr +=tem;
           postStr +="\"\r\n";
           postStr +="}";
           postStr +="}";
      
    client.print("POST /api/http HTTP/1.1\n");
    client.print("Host: ");
    client.print(server);
    client.print("\nContent-Type: application/json\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr); 
    //Serial.println(postStr);   
    //Serial.print(t); Serial.print("\t");
    //Serial.print(h); Serial.print("\t");
    delay(1000);  

   client.stop();
   Serial.println("waiting ...");    
   delay(1000);  
 } 
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  dht22_init();
  lcd_init();
  setDateDs1307();
  setup_bmp180();
  linksprite_io_init();
  pinMode(button,INPUT);
  display_logo();
  //backlightOff();
}

void loop()
{
  static unsigned long nowtime=0,lasttime=0,diffentime=0,updatetime=0;
  if(digitalRead(button)==0)
  {
    delay(10);
    if(digitalRead(button)==0)
    {
      backlightOn(); 
      diffentime = 0;
    }
    delay(140);
    if(digitalRead(button)==0)
    { 
      flag++;
      if(flag==4) flag = 1 ;
      while(!digitalRead(button));
      clearLCD(); 
    }
  }
  switch(flag)
  {
     case 1 : 
            display_logo(); 
            break;
     case 2 : 
            display_RTC(); 
            display_dht22(); 
            break;
     case 3 : 
            display_bmp180(); break;
     //case 4 : flag=0; UpdateToLinkSpriteIO(); break; 
  }
  nowtime = millis();
  diffentime += ( nowtime - lasttime );
  updatetime += ( nowtime - lasttime );
  lasttime = nowtime; 

  if(diffentime>5000) // Backlight turn time : 5s 
  {
    backlightOff();   
    diffentime = 0;
  }
  if(updatetime>60000) //update to LinkSprite IO time : 60s  
  {
    UpdateToLinkSpriteIO();
    //clearLCD(); 
    updatetime = 0;
  }
  delay(50); 
}
