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
double T, H, P; //T = temperature, H = humidity, P = pressure

byte temp1, temp2, temp3, hum1, hum2, press1, press2, press3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  
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
    T = sensorT() - c_time/100; P =sensorP() + c_time/100; H = sensorH() - c_time/1000;
    Serial.println(T);
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

float sensorT(){
  Wire.beginTransmission(bme);
  Wire.write(0xFA);
  Wire.endTransmission(false);

  Wire.requestFrom(bme, 3);
  if (Wire.available() == 3) { //you request 3 bytes each 8 bits so 24 bits (from registers 0xFA, 0xFB and 0xFC)
    temp1 = Wire.read();
    temp2 = Wire.read();
    temp3 = Wire.read();
  }
  else {
    Serial.println("I2C communication failed in temperature!");
  }

  return 22.9;
}

float sensorP(){
  Wire.beginTransmission(bme);
  Wire.write(0xF7);
  Wire.endTransmission(false);

  Wire.requestFrom(bme, 3);
  if (Wire.available() == 3) { //you request 3 bytes each 8 bits so 24 bits (from registers 0xF7, 0xF8 and 0xF9)
    press1 = Wire.read();
    press2 = Wire.read();
    press3 = Wire.read();
  }
  else {
    Serial.println("I2C communication failed in pressure!");
  }

  return 99.4;
}

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










