#include <Wire.h>

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
#define button_t 19
#define button_h 20

#define SCL 12
#define SDA 11

#define slave_addr 0xD0
#define hum_lsb 0xFE
#define hum_msb 0xFD
#define temp_xlsb 0xFC
#define temp_lsb 0xFB
#define temp_msb 0xFA
#define press_xlsb 0xF9
#define press_lsb 0xF8
#define press_msb 0xF7
int counter_display = 1, hundreds, decimal, integer;
unsigned long c_time, p_time = 0, c_time_display, p_time_display = 0;
bool button_t_h = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(dp, OUTPUT);

  pinMode(display1, OUTPUT);
  pinMode(display2, OUTPUT);
  pinMode(display3, OUTPUT);  

  Wire.begin(SDA, SCL); //initialize I2C communication as master
  Wire.beginTransmission(slave_addr);
  Wire.write(0xD0); //reset register
  Wire.write(0xB6); //hex decimal code to reset
  Wire.endTransmission();
}

void loop() {
  // put your main code here, to run repeatedly:
  c_time = millis();
  if(c_time - p_time >= 100){
    if(button_t_h ==  true){
      Wire.beginTransmission(slave_addr)
      Wire.write()
    }
    else if(button_t_h == false){
      Wire.beginTransmission(slave_addr)
      Wire.write()
    }
  }
  button_temp = digitalRead()

}




void separation(float num){
  int nums = num*100;
  hundreds = nums/100;
  decimal = (nums/10)%10;
  integer = nums%10;
  //Serial.printf("hundreds: %d, decimal: %d, integer: %d\n", hundreds, decimal , integer);
}


void use_display(){
  c_time_display = millis();
  if (c_time_display - p_time_display >= 5){
    ghosting();
    digitalWrite(display1, LOW);
    digitalWrite(display2, LOW);
    digitalWrite(display3, LOW);
    if (counter_display == 1) {
      digitalWrite(display1, HIGH);
      number(hundreds);
      counter_display++;
      p_time_display = c_time_display;
  }

  else if (counter_display == 2) {
      digitalWrite(display2, HIGH);
      number(decimal);
      counter_display++;
      p_time_display = c_time_display;
    }

  else if (counter_display == 3) {
      digitalWrite(display3, HIGH);
      number(integer);
      counter_display = 1;
      p_time_display = c_time_display;
  }
  }

}

void ghosting(){
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
  digitalWrite(dp, LOW);
}

void number(int chiffre){
  if (counter_display == 1){
    digitalWrite(dp, HIGH);
  }
  else if (counter_display != 1){
  digitalWrite(dp, LOW);
  }
  switch(chiffre){
    case 0:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, LOW);
    digitalWrite(G, HIGH);
    break;
    case 1:
    digitalWrite(A, HIGH);
    digitalWrite(B, LOW);
    digitalWrite(C, HIGH);
    digitalWrite(D, LOW);
    digitalWrite(E, LOW);
    digitalWrite(F, LOW);
    digitalWrite(G, LOW);
    break;
    case 2:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, LOW);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, LOW);
    break;
    case 3:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, LOW);
    digitalWrite(F, HIGH);
    digitalWrite(G, LOW);
    break;
    case 4:
    digitalWrite(A, HIGH);
    digitalWrite(B, LOW);
    digitalWrite(C, HIGH);
    digitalWrite(D, LOW);
    digitalWrite(E, LOW);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 5:
    digitalWrite(A, LOW);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, LOW);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 6:
    digitalWrite(A, LOW);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 7:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, LOW);
    digitalWrite(E, LOW);
    digitalWrite(F, LOW);
    digitalWrite(G, LOW);
    break;
    case 8:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 9:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, LOW);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
  }
}