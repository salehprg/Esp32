#include <Arduino.h>
#include <WiFi.h>
#include <ModbusRTU.h>

#define module_slaveid 1

#define additional_regCount 5

//set register offset to last register 
//ex : if register count is 16 we get 32 register (16 * 2) 
//and for mac address register we get 32 + reg_macaddress_1

#define reg_macaddress_1 1 
#define reg_macaddress_2 2 

#define tx_cntrlpin 18

#define reg_default_value 2
#define on_value 1
#define off_value 0

#define rellayCount 16
#define baudrate 9600

int pins[rellayCount] = {17 , 2 , 4 , 5 , 12 , 13 , 14 , 15 , 16 , 19 , 21 , 25 , 26 , 27 , 32 , 33};

ModbusRTU mb;

void setup() {

  int totalRegCount = (rellayCount * 2) + additional_regCount;
  int lastRegId = (rellayCount * 2);

  Serial.begin(baudrate , SERIAL_8N1);
  Serial2.begin(baudrate , SERIAL_8N1 , 22 , 23);

  for(int i = 0; i < rellayCount ;i++)
  {
    pinMode(pins[i] , OUTPUT);
    delay(20);
    digitalWrite(pins[i] , LOW );
    delay(20);
  }

  String macaddress = WiFi.macAddress();
  Serial.println(macaddress);

  macaddress.replace(":" , "");
  macaddress = macaddress.substring(macaddress.length() - 6 , macaddress.length());

  char char_array[macaddress.length() + 1];
  strcpy(char_array, macaddress.c_str());

  char * pEnd;
  int result = strtoul(char_array , &pEnd , 16);

  Serial.println(result);

  mb.begin(&Serial2 , tx_cntrlpin);

  mb.slave(module_slaveid);
  mb.addHreg(0 , reg_default_value , totalRegCount);

  mb.Hreg(lastRegId + reg_macaddress_1 , (uint16_t)(result / 10000));
  mb.Hreg(lastRegId + reg_macaddress_2 , (uint16_t)(result % 10000));

  Serial.println((uint16_t)(result / 10000));
  Serial.println((uint16_t)(result % 10000));

}

uint16_t regs[5];
int result = 0;

void loop() 
{
  mb.task();
  delay(10);

  for(int i = 0; i < rellayCount ;i++)
  {
    uint16_t value = mb.Hreg(i);
    uint16_t p_value = mb.Hreg(i + rellayCount);

    if(value == off_value)
    {
      digitalWrite(pins[i] , LOW);
      mb.Hreg(i + rellayCount , value);
    }
    else if (value == on_value)
    {
      digitalWrite(pins[i] , HIGH);
      mb.Hreg(i + rellayCount , value);
    }
      
    mb.Hreg(i , reg_default_value);

    delay(20);
  }
    
  //I edit library for simple code thus these while commented

  // while (mb.slave()) 
  // {
  //     mb.task();
  //     delay(5);
  // }

  // while (mb.slave()) 
  // {
  //     mb.task();
  //     delay(5);
  // }


  delay(100);

  yield();
}