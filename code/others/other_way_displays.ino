//esp32
			   //a,b,c,d,e,f,g
const byte segpins[7] = {4,5,6,7,15,16,17};
#define gate 2
#define gate2 42
#define gate3 41
unsigned long c_time, p_time = 0;
int timing = 1000;
bool state_display = false;

//10 rows 7 columns 	   a,b,c,d,e,f,g
const byte leds[10][7] = {{1,1,1,1,1,1,0},//0
            	   		      {0,1,1,0,0,0,0},//1
            	   		      {1,1,0,1,1,0,1},//2
            	   		      {1,1,1,1,0,0,1},//3
            	   		      {0,1,1,0,0,1,1},//4
            	   		      {1,0,1,1,0,1,1},//5
            	   		      {1,0,1,1,1,1,1},//6
            	     	      {1,1,1,0,0,0,0},//7
            	   		      {1,1,1,1,1,1,1},//8
            	   		      {1,1,1,1,0,1,1},//9
           		   		      };

void setup()
{
  Serial.begin(115200);
  //sizeof(segpins)/sizeof(segpins[0]) gives the size of array
  for(int i=0; i<7; i++){
  	pinMode(segpins[i], OUTPUT);
  }
  pinMode(gate, OUTPUT);
  digitalWrite(gate, LOW);

}

void loop()
{
  c_time = millis();
  if (c_time - p_time >= 100){
    Serial.println("hello");
    numbers(8);
    state_display = !state_display; 
    digitalWrite(gate, state_display);
    p_time = c_time;
  }
}

void numbers(int row){
  for (int j=0; j<7; j++){//sizeof() is gelijk aan aantal segmenten
    digitalWrite(segpins[j], leds[row][j]);//1=HIGH 0=LOW
  }
}