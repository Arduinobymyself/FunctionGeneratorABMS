 /*
  #####################################################################################
  #
  #  	File:               Arduino_Frequency_counter_Duty_Cycle_Meter_5.INO
  #     Processor:          Arduino UNO, MEGA ou Teensy++ 2.0      
  #  	Language:	    Wiring / C /Processing /Fritzing / Arduino IDE          
  #						
  #	Objectives:         Function Generator using XR-2206
  #										  
  #	Behavior:	    This FG will generate 3 wave forms like Sine, triangle and
  #                         square. It has DC, AC and TTL output. 
  #                         Frequency (coarse and fine) adjust.
  #                         Amplitude and OffSet adjust.
  #                         Range and Wave Form selection by a rotary switch
  #
  #			
  #     Author:             Marcelo Moraes
  #     Date:               03/23/2015	
  #     place:              Brazil, Sorocaba City
  #     website:            arduinobymyself.blogspot.com.br	
  #					
  #####################################################################################
 
  This project contains public domain code.
  The modification is allowed without notice.
*/

//Arduino frequency counter/duty cycle meter
//www.electronicsblog.net/
#include <Wire.h>
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
//#include <FreqCounter.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C LCD display

// definition
#define buttonUP 11
#define buttonDWN 12


// variables
int b=0;
int screen =0;

float Vsample=0;      // for amplitude
float AMPLITUDE=0.0;  // amplitude final value calculation
int VsensorPin = 0;    // arduino A0 pin voltage divider sensor
double sum = 0;
int count = 0;
float frequency = 0;
float period = 0;
float periodus = 0;
float periodms = 0;



// period of pulse accumulation and serial output, milliseconds
#define MainPeriod 100
long previousMillis = 0; // will store last time of the cycle end
unsigned long duration=0; // accumulates pulse width
unsigned int pulsecount=0;
unsigned long previousMicros=0;
unsigned long currentMicros;
unsigned long currentMillis;




  
  // new character definition
  byte customChar_1[8] = {
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111          };

  byte customChar_2[8] = {
	0b00010,
	0b00101,
	0b01000,
	0b10000,
	0b00000,
	0b00000,
	0b00000,
	0b00000          };
  byte customChar_3[8] = {
	0b00000,
	0b00000,
	0b10000,
	0b01000,
	0b00100,
	0b00010,
	0b00001,
	0b00000          };

  byte customChar_4[8] = {
	0b11100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00111         };
  byte customChar_5[8] = {
	0b00111,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b11100         };
  byte customChar_6[8] = {
	0b10000,
	0b01000,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00010,
	0b00001         };

  byte customChar_7[8] = {
	0b00001,
	0b00010,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b01000,
	0b10000       };
  byte customChar_8[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00001,
	0b00010,
	0b00100,
	0b01000,
	0b10000       };
  
  // arduino pins
  int sineWave = 3;          // sine wave signal from SW2C
  int triangleWave = 4;      // triangle wave signal from SW2C
  int squareWave = 5;        // square wave signal from SW2C
  int range_1 = 6;           // range 0 -100Hz
  int range_2 = 7;           // range 100Hz -1KHz
  int range_3 = 8;           // range 1KHz -10KHz
  int range_4 = 9;           // range 10KHz -100KHz
  int range_5 = 10;          // range 100KHz -1MHz





void myinthandler() // interrupt handler
{
  currentMicros = micros();
  duration += currentMicros - previousMicros;
  previousMicros = currentMicros;
  pulsecount++;
}



void setup()   {
  
  
  // Wire initialization
  Wire.begin();
  
  // LCD initialization
  lcd.init();          // I2C LCD initialization
  lcd.backlight();     // with Backlight
  lcd.clear();         // clearscreen
  
  // serial initialization
  Serial.begin(115200);
  

    
  //arduino pins definition
  pinMode(buttonUP, INPUT);
  digitalWrite(buttonUP, HIGH); //pull up resistor
  pinMode(buttonDWN, INPUT);
  digitalWrite(buttonDWN, HIGH);
  pinMode(sineWave, INPUT);
  digitalWrite(sineWave, HIGH);
  pinMode(triangleWave, INPUT);
  digitalWrite(triangleWave, HIGH);
  pinMode(squareWave, INPUT);
  digitalWrite(squareWave, HIGH);
  pinMode(range_1, INPUT);
  digitalWrite(range_1, HIGH);
  pinMode(range_2, INPUT);
  digitalWrite(range_2, HIGH);
  pinMode(range_3, INPUT);
  digitalWrite(range_3, HIGH);
  pinMode(range_4, INPUT);
  digitalWrite(range_4, HIGH);
  pinMode(range_5, INPUT);
  digitalWrite(range_5, HIGH);

  lcd.createChar(1, customChar_1);
  lcd.createChar(2, customChar_2);
  lcd.createChar(3, customChar_3);
  lcd.createChar(4, customChar_4);
  lcd.createChar(5, customChar_5);
  lcd.createChar(6, customChar_6);
  lcd.createChar(7, customChar_7);
  lcd.createChar(8, customChar_8);
  //lcd.createChar(9, customChar_9);
  //lcd.createChar(10, customChar_10);

  // initial messages on display
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    Initializing");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  for(int i = 1; i < 19; i++){
    lcd.setCursor(i,2);
    lcd.write((uint8_t)1);
    delay(250);
  }
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("      Welcome");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print(" Arduino By Myself");
  delay(1000);
  lcd.setCursor(0,2);
  lcd.print(" Function Generator");
  delay(2000);
  lcd.clear();
  attachInterrupt(0, myinthandler, RISING);
}


void loop(){ 
  /// screen modes
  switch (screen) {
  case 0:
  currentMillis = millis();
  if (currentMillis - previousMillis >= MainPeriod) 
  {
    previousMillis = currentMillis;   
    // need to bufferize to avoid glitches
    unsigned long _duration = duration;
    unsigned long _pulsecount = pulsecount;
    duration = 0; // clear counters
    pulsecount = 0;
    float Freq = 1e6 / float(_duration);
    Freq *= _pulsecount; // calculate F
    // output time and frequency data to RS232
    //Serial.print(currentMillis);
    //Serial.print(" "); // separator!
    //Serial.print(Freq);
    //Serial.print(" "); 
    //Serial.print(_pulsecount);
    //Serial.print(" ");
    //Serial.println(_duration);
    lcd.setCursor(0,0);
    lcd.print("  ArduinoByMyself   ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("Freq: ");
    lcd.print(Freq);
    lcd.print(" Hz");
    lcd.setCursor(0,3);
    lcd.print("Time: ");
    lcd.print((1/Freq)*1000);
    lcd.print(" ms  ");
  }
  //delay(500);
  break;
  
  case 1:
    lcd.setCursor(0,0);
    lcd.print("  ArduinoByMyself   ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    lcd.print("Period: ");
    if(periodus >= 1000.0){
      lcd.print(periodms);
      lcd.print(" ms     ");
    }
    else{
      lcd.print(periodus);
      lcd.print(" us     ");
    }
    lcd.setCursor(0,3); 
    lcd.print("                    "); 
  break; 
    
  case 2:
    lcd.setCursor(0,0);
    lcd.print("  ArduinoByMyself   ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    // check the range used from the switch S1B
    if(!digitalRead(range_1)) lcd.print("Range: 0 - 100Hz    ");
    if(!digitalRead(range_2)) lcd.print("Range: 100 - 1KHz   ");
    if(!digitalRead(range_3)) lcd.print("Range: 1K - 10KHz   ");
    if(!digitalRead(range_4)) lcd.print("Range: 10K - 100KHz ");
    if(!digitalRead(range_5)) lcd.print("Range: 100K - 1MHz  ");

    lcd.setCursor(0,3);
    // check the wave form used from teh switch S2C
    if(!digitalRead(sineWave)){
      lcd.print("WaveForm: Sin "); 
      lcd.write((uint8_t)6);
      lcd.write((uint8_t)7);
      lcd.write((uint8_t)6);
      lcd.print("   ");
    }
    if(!digitalRead(triangleWave)){
      lcd.print("WaveForm: Tri ");
      lcd.write((uint8_t)2);
      lcd.write((uint8_t)3);
      lcd.write((uint8_t)8);
      lcd.print("   ");
    }
    if(!digitalRead(squareWave)){
      lcd.print("WaveForm: Sqr ");
      lcd.write((uint8_t)4);
      lcd.write((uint8_t)5);
      lcd.write((uint8_t)4);
      lcd.print("   ");
    }
  break;
  
  case 3:
    for(int i=0;i<150;i++){
      Vsample+=analogRead(VsensorPin);  //read the voltage from the sensor
      delay(1); // 150 samples on 300ms
    }
    Vsample=Vsample/150; 
    AMPLITUDE=4.911*2*Vsample/1000; //  callibration // 4.911 from voltage div is eqv 512 in A0 reading  // multiply 2 to get actual voltage
    
    lcd.setCursor(0,0);
    lcd.print("  ArduinoByMyself   ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("Amplitude: ");
    lcd.print(AMPLITUDE);
    lcd.println(" V   ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
  break;
  }
  
  ///buttons /////////
  if(!digitalRead(buttonDWN)){
    screen++;
    if(screen > 3){
      screen=0 ;   
      lcd.clear();
    }
  }
  
  if(!digitalRead(buttonUP)){
    screen--;
    if(screen<0){
      screen=3 ;
      lcd.clear();  
    }
  }
}




