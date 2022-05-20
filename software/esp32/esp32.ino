/**
*
 *  @authors:  Mousa Alsayegh (amousa@pdx.edu),
 *  @date:    xx/xx/xxxx
 *
 *  @Description:
 *  
 *  
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

//Commented inputs are taken
//#define in4 34   //GPIO 34
#define in5 36   //GPIO 36
#define in6 39   //GPIO 39

//Commented outputs are taken
#define out1 4   //GPIO 4 Buzzer  
//#define out2 12  //GPIO 12  Green_status
//#define out3 13  //GPIO 13  Red_status
//#define out4 14  //GPIO 14    Red_power
//#define out5 15  //GPIO 15    Green_power
//#define out6 21  //GPIO 21    Blue_power
#define out7 25  //GPIO 25
#define out8 26  //GPIO 26
#define out9 27  //GPIO 27
#define out10 32 //GPIO 32
//#define out11 33 //GPIO 33  Blue_status

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
