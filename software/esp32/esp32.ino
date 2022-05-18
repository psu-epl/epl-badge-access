/**
*
 *  @authors:  Mousa Alsayegh (amousa@pdx.edu),
 *  @date:    xx/xx/xxxx
 *
 *  @Description:
 *  
 *  
 */
/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Wire.h>
WiFiMulti WiFiMulti;


//***********GPIO Port Definitions***********
//I2C
#define scl 22   //GPIO 22
#define sda 23   //GPIO 23

//UART
#define RX 16    //RX
#define TX 17    //TX

//SPI
#define MISO 19  //MISO.
#define MOSI 23  //MOSI
#define SCLK 18  //SCLK

//Inputs
#define in4 34   //GPIO 34
#define in5 36   //GPIO 36
#define in6 39   //GPIO 39

//Outputs
#define out1 4   //GPIO 4 Buzzer  
//#define out2 12  //GPIO 12  Green
//#define out3 13  //GPIO 13  Red
#define out4 14  //GPIO 14
#define out5 15  //GPIO 15
#define out6 21  //GPIO 21
#define out7 25  //GPIO 25
#define out8 26  //GPIO 26
#define out9 27  //GPIO 27
#define out10 32 //GPIO 32
//#define out11 33 //GPIO 33  Blue

void setup()
{   
  Serial.begin(115200);   //Open serial port and set data rate
  delay(10);
  
  //***********GPIO Assignments*********************************
/*
  pinMode(in34, INPUT);
  pinMode(in36, INPUT);  
  pinMode(in39, INPUT);
  pinMode(out4, OUTPUT);
  pinMode(out12, OUTPUT);
  pinMode(out13, OUTPUT);
  pinMode(out14, OUTPUT);
  pinMode(out15, OUTPUT);
  pinMode(out21, OUTPUT);
  pinMode(out25, OUTPUT);
  pinMode(out26, OUTPUT);
  pinMode(out27, OUTPUT);
  pinMode(out32, OUTPUT);
  pinMode(out33, OUTPUT);
 */
}



//***********Loop*********************************
void loop(){

}
