/************************************************************************
*
* PMOD MIC3 Speed Test
* 
* The PMOD MIC3 uses a ADCS7476AIMFX/NOPB to convert 
* the analog signal from the micrphone to a digital value.
* The range of clock frequencies for this ADC is 10 kHz to 20MHz
* 
* Hz Calculation doesnt work if the time taken to take all samples
* is above 1 second
* 
* Due to the speed of the SPI, and the fact that doubles are 32
* bits on arduinos, calculating Hz as SAMPLES/delta doesn't work for small  
* sample sizes as the time delta is measured in microseconds, so it needs
* to be coverted to seconds with delta/1000000, which often results in
* just 0 for small sample sizes as float doesnt seem to provide the
* necessary resolution
* 
* @ 1000000 Clock frequency, a sample rate of around 41k +-200hz can be
* seen on the MKR ZERO board. 100KHz @ 8000000 on the same board
************************************************************************/

/*
  Wiring
  Module  <-------> Arduino
  VCC     to        VCC
  GND     to        GND
  SCK     to        9   (SCK)
  MISO    to        10  (MISO)
  SS      to        6
*/

#include <SPI.h> // call library

#define SS 6 // Assignment of the CS pin
#define ADR 0x00

#define SAMPLES 100000
#define CLOCK_FREQUENCY 1000000 /* set between 10kHz - 20MHz */

SPISettings pmod_settings(CLOCK_FREQUENCY, MSBFIRST, SPI_MODE0);
byte res[2] = {0};

uint32_t analog = 0;

// Timing variables
uint32_t start = 0;
uint32_t delta = 0;

void setup(){
  Serial.begin(115200);
  SPI.begin();
  pinMode(SS, OUTPUT);
}

void loop(){
  start = micros();
  for(int i = 0; i < SAMPLES; i++){
    SPI.beginTransaction(pmod_settings);
    
    digitalWrite(SS, LOW);
    res[0] = SPI.transfer(ADR);
    res[1] = SPI.transfer(ADR);
    digitalWrite(SS, HIGH);
    
    analog = res[0] << 8 | res[1];
  }
  SPI.endTransaction();
  delta = micros() - start;

  Serial.print("Execution time is:\t");Serial.print(delta);Serial.println(" us");
  Serial.print("Sampling Frequency:\t");Serial.println(float(SAMPLES/(delta/1000000.0f)));
  Serial.println();
  
  delay(2000);
}
