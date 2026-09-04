const byte letters[26][5] = {
  {0x7E,0x11,0x11,0x11,0x7E}, // A
  {0x7F,0x49,0x49,0x49,0x36}, // B
  {0x3E,0x41,0x41,0x41,0x22}, // C
  {0x7F,0x41,0x41,0x22,0x1C}, // D
  {0x7F,0x49,0x49,0x49,0x41}, // E
  {0x7F,0x09,0x09,0x09,0x01}, // F
  {0x3E,0x41,0x49,0x49,0x7A}, // G
  {0x7F,0x08,0x08,0x08,0x7F}, // H
  {0x00,0x41,0x7F,0x41,0x00}, // I
  {0x20,0x40,0x41,0x3F,0x01}, // J
  {0x7F,0x08,0x14,0x22,0x41}, // K
  {0x7F,0x40,0x40,0x40,0x40}, // L
  {0x7F,0x02,0x0C,0x02,0x7F}, // M
  {0x7F,0x04,0x08,0x10,0x7F}, // N
  {0x3E,0x41,0x41,0x41,0x3E}, // O
  {0x7F,0x09,0x09,0x09,0x06}, // P
  {0x3E,0x41,0x51,0x21,0x5E}, // Q
  {0x7F,0x09,0x19,0x29,0x46}, // R
  {0x46,0x49,0x49,0x49,0x31}, // S
  {0x01,0x01,0x7F,0x01,0x01}, // T
  {0x3F,0x40,0x40,0x40,0x3F}, // U
  {0x1F,0x20,0x40,0x20,0x1F}, // V
  {0x3F,0x40,0x38,0x40,0x3F}, // W
  {0x63,0x14,0x08,0x14,0x63}, // X
  {0x07,0x08,0x70,0x08,0x07}, // Y
  {0x61,0x51,0x49,0x45,0x43}  // Z
};

const byte numbers[10][5] = {
  {0x3E,0x51,0x49,0x45,0x3E}, // 0
  {0x00,0x42,0x7F,0x40,0x00}, // 1
  {0x42,0x61,0x51,0x49,0x46}, // 2
  {0x21,0x41,0x45,0x4B,0x31}, // 3
  {0x18,0x14,0x12,0x7F,0x10}, // 4
  {0x27,0x45,0x45,0x45,0x39}, // 5
  {0x3C,0x4A,0x49,0x49,0x30}, // 6
  {0x01,0x71,0x09,0x05,0x03}, // 7
  {0x36,0x49,0x49,0x49,0x36}, // 8
  {0x06,0x49,0x49,0x29,0x1E}  // 9
};

const byte degree[5]  = {0x06,0x09,0x09,0x06,0x00}; // °
const byte percent[5] = {0x23,0x13,0x08,0x64,0x62}; // %
const byte minus[5]   = {0x08,0x08,0x08,0x08,0x08}; // -
const byte dot[5]     = {0x00,0x60,0x60,0x00,0x00}; // .
const byte space[5]   = {0x00,0x00,0x00,0x00,0x00};


#include <Wire.h>
#include <string>
#define SDA 16
#define SCL 15

/*SLAVE ADDRESSES
  Oled display adress is 0 1 1 1 1 0 SA0 R/W ||| SA0 can be 0 or 1 and R/W = 0 ==> write mode, R/W = 1 ==> read mode 
  hexa for 0 1 1 1 1 0 0 is 0x3C*/
#define oled 0x3C

/*sensor adress is 1 1 1 0 1 1 x || x = 0 if SDO to GND and x = 1 if SDO to Vddio
  Write is 0 and read is 1 ==> 1 1 1 0 1 1 x R/W */
#define bme 0x76 //SDO to GND 1 1 1 0 1 1 0

int lap = 100;
unsigned long c_time, p_time = 0;
double T, H, P, t_fine; //T = temperature, H = humidity, P = pressure

byte temp1, temp2, temp3, hum1, hum2, press1, press2, press3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(SDA, SCL);

  /*
  Wire.beginTransmission(bme);
  Wire.write(0xF2); //for humidity
  Wire.write(0x01);
  Wire.endTransmission(false);
  */

  Wire.beginTransmission(bme);
  Wire.write(0xF4); //for temp, pressure and normal mode
  Wire.write(0x27); 
  Wire.endTransmission();
  
  Wire.beginTransmission(oled);
  Wire.write(0x00);
  //thank god for the software flowchart page 64
  Wire.write(0xA8); Wire.write(0x3F);
  Wire.write(0xD3); Wire.write(0x00); //0x00 is a parameter of D3
  Wire.write(0x40); 
  Wire.write(0xA1);
  Wire.write(0xC8); 
  Wire.write(0xDA); Wire.write(0x12); 
  Wire.write(0x81); Wire.write(0x7F);
  Wire.write(0xA4); Wire.write(0xA6);
  Wire.write(0xD5); Wire.write(0x80);
  Wire.write(0x8D); Wire.write(0x14);
  Wire.write(0xAF);
  Wire.write(0x20); Wire.write(0x00); //cofigures horizontal mode

  Wire.endTransmission();

  clear_screen();
  display_text(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  c_time = millis();
  if (c_time - p_time >= lap) {
    sensor(); H = sensorH() - c_time/1000;
    display(T, P, H);
    p_time = c_time;
  }
}

/*the display needs a control byte which consist of Co D/C 0 0 0 0 0 0
  Co is 0 so the following information contains only bits
  D/C will tell if you want to draw on screen or configure it
  D/C = 0 means configuring it ||| D/C = 1 means putting something on the screen
  for commmand use 0 0 0 0 0 0 0 0 or 0x00||| for writing use 0 1 0 0 0 0 0 0 or 0x40*/
void display(double T, float P, float H){
  char text[3][16];

  snprintf(text[0], sizeof(text[0]), "%.1f", T);
  snprintf(text[1], sizeof(text[1]), "%.1f", H);
  snprintf(text[2], sizeof(text[2]), "%.1f", P);

  for (int i = 0; i < 3; i++){
    int page = i*2;
    Wire.beginTransmission(oled);
    Wire.write(0x00);

    Wire.write(0x21);
    Wire.write(0x5F);
    Wire.write(0x7F);

    Wire.write(0x22);
    Wire.write(0x02 + page);
    Wire.write(0x02 + page);
    Wire.endTransmission();

    Wire.beginTransmission(oled);
    Wire.write(0x40);
    for (int j = 0; text[i][j] != '\0'; j++){
      int index = text[i][j] - '0';
      for (int k = 0; k < 5; k++){
        if (text[i][j] == '.'){
          Wire.write(dot[k]);
        }
        else if (text[i][j] == '-'){
          Wire.write(minus[k]);
        }
        else {
          Wire.write(numbers[index][k]);
        }
      }      
    }
    Wire.endTransmission();
  }
}

void display_text(){
  const char* text[] = {"TEMPERATURE", "HUMIDITY", "PRESSURE"};

  for (int i = 0; i < 3; i++) {
    int page = i*2;
    Wire.beginTransmission(oled);
    Wire.write(0x00);

    Wire.write(0x21);
    Wire.write(0x00);
    Wire.write(0x7F);
    
    Wire.write(0x22);
    Wire.write(0x02 + page);
    Wire.write(0x02 + page);  
    Wire.endTransmission();

    Wire.beginTransmission(oled);
    Wire.write(0x40);
    for (int j = 0; text[i][j] != '\0'; j++) { // '\0' means keep going till end character is reached
      int index = text[i][j] - 'A';
      for (int k = 0; k < 5; k++){
        Wire.write(letters[index][k]);
      }
      Wire.write(0x00); //Space between letters
      Wire.write(0x00);
    }
    Wire.endTransmission();
  } 
}

void clear_screen() {
  int col = 0;
  Wire.beginTransmission(oled);
  Wire.write(0x00);

  Wire.write(0x21);  // set column range
  Wire.write(0x00);  // start column = 0
  Wire.write(0x7F);  // end column = 127
  
  Wire.write(0x22);  // set page range
  Wire.write(0x00);  // start page = 0
  Wire.write(0x07);  // end page = 7

  Wire.endTransmission();

  for (int i = 1; i < 17; i++){
    Wire.beginTransmission(oled);
    Wire.write(0x40);
    for (col; col < 64*i; col++){
      Wire.write(0x00);
    }
    Wire.endTransmission();
  }
}

void sensor(){
  uint32_t temp, press, hum;
  Wire.beginTransmission(bme);
  Wire.write(0xF7);
  Wire.endTransmission(false);

/*You request 8 bytes because temp and pressure have 3 bytes each and humidity has 2 bytes

*/
  Wire.requestFrom(bme, 8);
  if (Wire.available() == 8){
    press1 = Wire.read(); //pressure_msb
    press2 = Wire.read(); //pressure_lsb
    press3 = Wire.read(); //pressure_xlsb
    press = ((uint32_t)(press1 << 12) | (uint32_t)(press2 << 4) | (uint32_t)(press3 >> 4));//adc_P

    temp1 = Wire.read(); //temperature_msb
    temp2 = Wire.read(); //temperature_lsb
    temp3 = Wire.read(); //temperature_xlsb
    temp = ((uint32_t)(temp1 << 12) | (uint32_t)(temp2 << 4) | (uint32_t)(temp3 >> 4));//adc_T

    hum1 = Wire.read(); //humidity_msb
    hum2 = Wire.read(); //humidity_lsb
    hum = ((uint32_t)(hum1 << 8) | hum2);//adc_H

    Serial.print(press1);
  }
  else {
    Serial.println("I2C communication failed!");
  }
  calibration_T(temp);
  calibration_P(press);
}

void calibration_T(uint32_t temp){
  uint16_t dig_T1; int16_t dig_T2, dig_T3;
  Wire.beginTransmission(bme);
  Wire.write(0x88);
  Wire.endTransmission();

  Wire.requestFrom(bme, 6);
  if (Wire.available() == 6){ //first read is lsb and second read is msb
    byte t1_lsb = Wire.read(); 
    byte t1_msb = Wire.read();
    byte t2_lsb = Wire.read();
    byte t2_msb = Wire.read();
    byte t3_lsb = Wire.read();
    byte t3_msb = Wire.read();

    dig_T1 = ((uint16_t)t1_msb << 8 | t1_lsb);
    dig_T2 = (int16_t)((uint16_t)t2_msb << 8 | t2_lsb);
    dig_T3 = (int16_t)((uint16_t)t3_msb << 8 | t3_lsb);
  }
  else {
    Serial.print("Cannot access calibration_T");
  }

  double var1, var2;

  var1 = ((temp/16384.0) - (dig_T1/1024.0))*dig_T2;
  var2 = ((temp/131072.0) - (dig_T1/8192.0));
  var2 = var2 * var2 * dig_T3;

  t_fine = var1 + var2;
  T = t_fine/5120.0;
}

void calibration_P(uint32_t press){
  uint16_t dig_P1;
  int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
  Wire.beginTransmission(bme);
  Wire.write(0x8E);
  Wire.endTransmission();

  Wire.requestFrom(bme, 18);
  if (Wire.available() == 18){
    byte p1_lsb = Wire.read(); 
    byte p1_msb = Wire.read();
    byte p2_lsb = Wire.read();
    byte p2_msb = Wire.read();
    byte p3_lsb = Wire.read();
    byte p3_msb = Wire.read();
    byte p4_lsb = Wire.read(); 
    byte p4_msb = Wire.read();
    byte p5_lsb = Wire.read();
    byte p5_msb = Wire.read();
    byte p6_lsb = Wire.read();
    byte p6_msb = Wire.read();
    byte p7_lsb = Wire.read(); 
    byte p7_msb = Wire.read();
    byte p8_lsb = Wire.read();
    byte p8_msb = Wire.read();
    byte p9_lsb = Wire.read();
    byte p9_msb = Wire.read();

    dig_P1 = ((uint16_t)(p1_msb << 8) | p1_lsb);
    dig_P2 = (int16_t)((uint16_t)p2_msb << 8 | p2_lsb);
    dig_P3 = (int16_t)((uint16_t)p3_msb << 8 | p3_lsb);
    dig_P4 = (int16_t)((uint16_t)p4_msb << 8 | p4_lsb);
    dig_P5 = (int16_t)((uint16_t)p5_msb << 8 | p5_lsb);
    dig_P6 = (int16_t)((uint16_t)p6_msb << 8 | p6_lsb);
    dig_P7 = (int16_t)((uint16_t)p7_msb << 8 | p7_lsb);
    dig_P8 = (int16_t)((uint16_t)p8_msb << 8 | p8_lsb);
    dig_P9 = (int16_t)((uint16_t)p9_msb << 8 | p9_lsb);
  }
  else {
    Serial.print("Cannot access calibration_P");
  }
  double var1, var2, p;

  var1 = t_fine/2.0 - 64000.0;
  var2 = var1*var1*((double)dig_P6)/32768.0;
  var2 = var2 + var1 * ((double)dig_P5)/2.0;
  var2 = (var2/4.0) + (((double)dig_P4)*65536.0);
  var1 = (((double)dig_P3)*var1*var1/524288.0 + ((double)dig_P2)*var1)/524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
  if (var1 == 0.0){
    return;
  }
  p = 1048576.0 - (double)press;
  p = (p - (var2/4096.0))*6250.0 / var1;
  var1 = ((double)dig_P9)*p*p/2147483648.0;
  var2 = p*((double)dig_P8)/32768.0;
  p = p + (var1 + var2 + ((double)dig_P7))/16.0;
  P = p/100.0; //pressure in hPA
}
//Another time this is nasty
/*
void calibration_H(){
  Wire.beginTransmission(bme);
  Wire.write(0xA1);
  Wire.endTransmission();

  Wire.requestFrom(bme, 1);
  if (Wire.available() == 1){
    byte h1 = Wire.read();

    uint8_t dig_H1 = (uint8_t)(h1);
  }

  Wire.beginTransmission(bme);
  Wire.write(0xE1);
  Wire.endTransmission();

  Wire.requestFrom(bme, 7);
  if (Wire.available() == 7){
    byte h2_lsb = Wire.read();
    byte h2_msb = Wire.read();
    byte h3 = Wire.read();
    byte h4_msb = Wire.read();
    byte h4_5_shared = Wire.read(); 
    byte h5_msb = Wire.read(); 
    byte h6 = Wire.read();

    byte shared4 = (h4_5_shared & 0x0F); int16_t dig_H4 = (int16_t)((int16_t)h4_msb << 4 | shared4);
    byte shared5 = (h4_5_shared >> 4);

    int16_t dig_H2 = (int16_t)((uint16_t)h2_msb << 8 | h2_lsb);
    uint8_t dig_H3 = (uint8_t)(h3);
    int16_t dig_H4 = (int16_t)((int16_t)(int8_t)h4_msb << 4 | shared4);
    int16_t dig_H5 = (int16_t)((int16_t)(int8_t)h5_msb << 4 | shared5);
    int8_t dig_H6 = (int8_t)(h6); 
  }
  else {
    Serial.print("Cannot access calibration_H");
  }
}
*/

float sensorH(){
  Wire.beginTransmission(bme);
  Wire.write(0xFD);
  Wire.endTransmission(false);

  Wire.requestFrom(bme, 2);
  if (Wire.available() == 2) { //you request 2 bytes each 8 bits so 16 bits (from registers 0xFD and 0xFE)
    hum1 = Wire.read();
    hum2 = Wire.read();

  }
  else {
    Serial.println("I2C communication failed in humidity!");
  }

  return 58.5;
}









