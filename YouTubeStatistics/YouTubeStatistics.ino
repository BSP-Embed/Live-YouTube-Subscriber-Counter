/*******************************************************************
 *  Main application program/sketch for reading YouTube Statistics *
 *  written by BSP Embed (BSP-Embed on Github)                     *
 *  YouTube & Arduino Jason Library are required                   *
 *  Demo Available on YouTube BSP Embed                            *
 *******************************************************************/
#include <YoutubeApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define API_KEY     "YOUR_API_KEY"                         /* your google apps API Token */
#define CHANNEL_ID  "UCZpgTN2ZoU7c4iWRQmk2bTg"             /* Youtube Channel ID */

#define api_mtbs    60000       /* Frequency of reading */
#define NUM_SPACE   0xff
#define LOG_PARAMETERS

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

char ssid[] =     "Bspembed";            /* your network SSID (name) */
char password[] = "WiFi-PASSWORD";                  

int BCDPins[] =   {12, 13, 14, 15};       /* define port pins */
int DIGIPins[] =  {0,2,4,5};
int BuzPin =      16;

uint8_t units;
uint8_t tens;
uint8_t hunds;
uint8_t ths;
uint8_t DispCount = 0;
long dura;
long NewSubs, OldSubs = 0;
long api_lasttime;          /* last time api request has been done */

void setup() {
  int i;
  for (i = 0; i < 4; i++) {
    pinMode(BCDPins[i], OUTPUT);
    pinMode(DIGIPins[i], OUTPUT);
    digitalWrite(BCDPins[i], 0);
    digitalWrite(DIGIPins[i], 0);
  }
  pinMode(BuzPin, OUTPUT);
  digitalWrite(BuzPin, 0);   
  Serial.begin(115200);
  ConnectAP();
  Beep(100);  
  dura = millis();
}

void loop() {
  if (millis() > api_lasttime + api_mtbs)  {
    if(api.getChannelStatistics(CHANNEL_ID)) {
      #ifdef LOG_PARAMETERS
        LogToSerial();
      #endif
      NewSubs = api.channelStats.subscriberCount;
      if (NewSubs != OldSubs) {
        IntToDigit(NewSubs);
        Beep(100);
        OldSubs = NewSubs;
      }
    }
    api_lasttime = millis();
  }
  if ((millis() -  dura) >= 2) {  /* Refresh Display */
    MultiPlex(DispCount); 
    dura = millis();   
  }
}
/**************************************/
/* Log YouTube Stats to serial Monitor*/
/**************************************/
void LogToSerial(void) {
   Serial.println("---------Stats---------");
   Serial.print("Subscriber Count: ");
   Serial.println(api.channelStats.subscriberCount);
   Serial.print("View Count: ");
   Serial.println(api.channelStats.viewCount);
   Serial.print("Comment Count: ");
   Serial.println(api.channelStats.commentCount);
   Serial.print("Video Count: ");
   Serial.println(api.channelStats.videoCount);
   Serial.println("------------------------");
}
/**************************************/
/* Connect to WiFi Router             */
/* (Access Point)                     */
/**************************************/
void ConnectAP(void) {
  WiFi.mode(WIFI_STA);    /* Set WiFi to station mode */
  WiFi.disconnect();     /* disconnect from an AP if it was Previously  connected*/
  delay(100);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}
/**************************************/
/* Convert int to sepearte digits     */
/* With Leading white space           */
/**************************************/
void IntToDigit(uint16_t temp){
  uint8_t dig;
  
  dig = temp / 1000;
  temp -= (temp / 1000) * 1000;
  
  if(dig == 0)
    ths = NUM_SPACE;
  else
    ths = dig;
    
  dig = temp / 100;
  temp -= (temp / 100)*100;
  if((ths == 0xff) && (dig == 0))
    hunds = NUM_SPACE;
  else
    hunds = dig;
    
   dig = temp / 10;
   temp -= (temp / 10) * 10;
   if((hunds == 0xff) && (dig == 0))
    tens = NUM_SPACE;
  else
    tens = dig;
    
  dig = temp % 10;
  units = dig;
}
/**************************************/
/* Multiplex seven segment           */
/**************************************/
void MultiPlex(uint8_t dig_count) {
  if(DispCount++ > 3) DispCount = 0;
  switch (dig_count) {
    case 0:
        digitalWrite(DIGIPins[0], 0);
        digitalWrite(DIGIPins[1], 0);
        digitalWrite(DIGIPins[2], 0);
        digitalWrite(DIGIPins[3], 1);
        DispDigit(units);
        break;
    case 1:
        digitalWrite(DIGIPins[0], 0);
        digitalWrite(DIGIPins[1], 0);
        digitalWrite(DIGIPins[2], 1);
        digitalWrite(DIGIPins[3], 0);
        DispDigit(tens);
        break;
    case 2:
        digitalWrite(DIGIPins[0], 0);
        digitalWrite(DIGIPins[1], 1);
        digitalWrite(DIGIPins[2], 0);
        digitalWrite(DIGIPins[3], 0);
        DispDigit(hunds);
        break;
    case 3:
        digitalWrite(DIGIPins[0], 1);
        digitalWrite(DIGIPins[1], 0);
        digitalWrite(DIGIPins[2], 0);
        digitalWrite(DIGIPins[3], 0);
        DispDigit(ths);
        break;
   }
}
/**************************************/
/* Set a BCD Number for a Digit       */
/**************************************/
void DispDigit(uint8_t x) {
  switch(x) {
    case 0: digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 0);
            digitalWrite(BCDPins[2], 0);
            digitalWrite(BCDPins[3], 0);
            break;
    case 1: digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 0);
            digitalWrite(BCDPins[2], 0);
            digitalWrite(BCDPins[3], 1);
            break;
   case 2: digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 0);
            digitalWrite(BCDPins[2], 1);
            digitalWrite(BCDPins[3], 0);
            break;
    case 3: digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 0);
            digitalWrite(BCDPins[2], 1);
            digitalWrite(BCDPins[3], 1);
            break;  
    case 4: digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 1);
            digitalWrite(BCDPins[2], 0);
            digitalWrite(BCDPins[3], 0);
            break;
    case 5: digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 1);
            digitalWrite(BCDPins[2], 0);
            digitalWrite(BCDPins[3], 1);
            break;
   case 6:  digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 1);
            digitalWrite(BCDPins[2], 1);
            digitalWrite(BCDPins[3], 0);
            break;
    case 7: digitalWrite(BCDPins[0], 0);
            digitalWrite(BCDPins[1], 1);
            digitalWrite(BCDPins[2], 1);
            digitalWrite(BCDPins[3], 1);
            break;   
    case 8: digitalWrite(BCDPins[0], 1);
            digitalWrite(BCDPins[1], 0);
            digitalWrite(BCDPins[2], 0);
            digitalWrite(BCDPins[3], 0);
            break;
    case 9: digitalWrite(BCDPins[0], 1);
            digitalWrite(BCDPins[1], 0);
            digitalWrite(BCDPins[2], 0);
            digitalWrite(BCDPins[3], 1);
            break; 
    case NUM_SPACE:
            digitalWrite(BCDPins[0], 1);
            digitalWrite(BCDPins[1], 1);
            digitalWrite(BCDPins[2], 1);
            digitalWrite(BCDPins[3], 1);
            break;               
  }
}
/**************************************/
/* Beep for indication                */
/**************************************/
void Beep(uint16_t OnTime) {
  digitalWrite(BuzPin, 1);
  delay(OnTime);
  digitalWrite(BuzPin, 0);
}


