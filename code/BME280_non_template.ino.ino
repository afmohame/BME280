#include <Wire.h>

#define slave_addr 0x76 //datasheet page 32. The BME280 is conected to GND
#define scl_pin 12 
#define sda_pin 11
#define data 0xFA

#define display1 2
#define display2 42
#define display3 41

#define A 4 //don't use gpio 0 because it a BOOT strapping pin 
#define B 5
#define C 6
#define D 7
#define E 15
#define F 16
#define G 17
#define dp 18

unsigned long c_time, p_time;
int hundreds, decimal, integer;;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(sda_pin, scl_pin); //initialize i2c communication with 
}

void loop() {
  // put your main code here, to run repeatedly:
  c_time = millis();
  if (c_time - p_time >= 1000){
    wire_function();
    p_time = c_time;
  }

}

void wire_function(){
  Wire.beginTransmission(slave_addr);
  Wire.write(data);
  Wire.endTransmission(false);

  Wire.requestFrom(slave_addr, 5);
  if (Wire.available() == 5){
    Serial.println("i am in");
    byte temp_msb = Wire.read();
    byte temp_lsb = Wire.read();
    byte temp_xlsb = Wire.read();
    byte humid_msb = Wire.read();
    byte humid_lsb = Wire.read();
  }
}