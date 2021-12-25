#include <Adafruit_ADS1X15.h>
#include <SPI.h>

#define VREF 2.9
#define voltageSense  200

#define ADC_10_SCALE 1023
#define ADC_16_SCALE 65533

#define voltageSensor_ads_pin 0

Adafruit_ADS1115 ads;

int voltageZero = 512;


void setup(void)
{
  Serial.begin(9600);

  Serial.println("Start");

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  
  
  ads.setGain(GAIN_ONE);

  uint32_t acc = 0;
	for (int i = 0; i < 10; i++) {
		acc += ads.readADC_SingleEnded(voltageSensor_ads_pin);
		delay(10);
	}
	voltageZero = acc / 10;

}

float getRMS_ADS(int8_t ADSpin , int32_t zeroREF , float sense) 
{
	double high = 0;
	double now;

	for(int i = 0; i < 20; i++) 
  {
		now = ads.readADC_SingleEnded(ADSpin);
       
		if(now > high)
    {
      high = now;
    }
	}

	float rms = high / ADC_16_SCALE * VREF * sense;
	return rms;

}

void loop()
{

  float volts0 = getRMS_ADS(voltageSensor_ads_pin , voltageZero , voltageSense);
  //int adc = ads.readADC_SingleEnded(voltageSensor_ads_pin);

  Serial.println(volts0);

  delay(100);
}