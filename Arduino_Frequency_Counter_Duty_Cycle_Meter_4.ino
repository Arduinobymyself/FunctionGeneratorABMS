 /*
  #####################################################################################
  #
  #  	File:               Arduino_Frequency_counter_Duty_Cycle_Meter_4.INO
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
  
  
  Used pins on Arduino
  Button UP        - D2, used as interruption 0
  Button DWN       - D3, usedas interruption 1
  
  Frequency sense  - D5, used by the frequency counter library
  
  Voltmeter        - A0, from the resistor divider
  LCD SDA/SCL      - A4/A5, to the LCD I2C 20x4
  
  Sine wave        - D11, from rotary switch 
  Triangle wave    - D12, from rotary switch 
  Square wave      - D13, from rotary switch 
  
  Range 0 - 100    - D6, from rotary switch 
  Range 100 - 1K   - D7, from rotary switch 
  Range 1K - 10K   - D8, from rotary switch 
  Range 10K - 100K - D9, from rotary switch 
  Range 100K - 1M  - D10, from rotary switch 
  
  You can see the complete project at arduinobymyself.blogspot.com.br or at instructable.com/BIGDOG1971
  emails to arduinobymyself@gmail.com
  */

  //####################################################################################################
  //all libraries included
  #include <Wire.h>
  #include "Arduino.h"
  #include <LiquidCrystal_I2C.h>
  #include <FreqCounter.h>

  //I2C LCD constructor
  LiquidCrystal_I2C lcd(0x27, 20, 4);


  //some definitions
  #define buttonUP 2
  #define buttonDWN 3
  #define sineWave  11          // sine wave signal from SW2C
  #define triangleWave  12      // triangle wave signal from SW2C
  #define squareWave 13         // square wave signal from SW2C
  #define range_1 6            // range 0 -100Hz
  #define range_2 7            // range 100Hz -1KHz
  #define range_3 8            // range 1KHz -10KHz
  #define range_4 9            // range 10KHz -100KHz
  #define range_5 10           // range 100KHz -1MHz


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
  int samples = 300;
  
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


void ISRButtonUP(){
  screen = screen - 1;
  if(screen<0){
    screen = 2 ;
    //lcd.clear();  
  }
}

void ISRButtonDWN(){
  screen = screen + 1;
  if(screen > 2){
    screen = 0 ;   
    //lcd.clear();
  }
}  
  

void setup()   {
  // Wire initialization
  Wire.begin();
  
  // LCD initialization
  lcd.init();          // I2C LCD initialization
  lcd.backlight();     // with Backlight
  lcd.clear();         // clearscreen
  
  // serial initialization
  Serial.begin(9600);
    
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
  
  attachInterrupt(0, ISRButtonUP, FALLING);
  attachInterrupt(1, ISRButtonDWN, FALLING);
}


void loop(){ 
  /// screen modes to display teh information
  switch (screen) {
  case 0://shows the frequency and period
    FreqCounter::f_comp=10;    // Cal Value / Calibrate with professional Freq Counter
    FreqCounter::start(1000);  // values of 10, 100 or 1000 ms are practicable for a resolution of 100, 10 and 1 Hz 
    while (FreqCounter::f_ready == 0)  // wait until counter ready
    frequency=FreqCounter::f_freq;   //get the frequency value
    period = (1/frequency)*1000000;  //calculates the periodo value
    
    //lcd.clear();                //clears the LCD screen
    lcd.setCursor(0,0);
    lcd.print("  ArduinoByMyself   ");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);         //initial cursor position
    if(frequency > 999999)
    {
      lcd.print("Freq: ");
      lcd.print(frequency/1000000,2);
      lcd.print(" MHz     ");
    }
    else if(frequency > 999 && frequency <= 999999 )
    {
      lcd.print("Freq: ");
      lcd.print(frequency/1000,2);
      lcd.print(" KHz     ");
    }
    else
    {
      lcd.print("Freq: ");
      lcd.print(frequency,2);
      lcd.print(" Hz      ");
    }
    lcd.setCursor(0,3);
    if(period > 999)
    {
      lcd.print("Time: ");
      lcd.print(period/1000,2);
      lcd.print(" ms      ");
    }
    else
    {
      lcd.print("Time: ");
      lcd.print(period,2);
      lcd.print(" us      ");
    }
    //delay(1);  //screen update period
  break;
    
  case 1://shows the frequency range
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
  
  case 2://shows the signal amplitude
    for(int i=0;i<samples;i++){
      Vsample += analogRead(VsensorPin);  //read the voltage from the sensor and do the sum to the 150 samples
      delayMicroseconds(5000); // one sample at 500us, total delay is equal to samples * 500us
    }
    Vsample=Vsample/samples;//total voltage sum dived by the sample number is the average voltage
    //  callibration 
    // 4.911 from voltage div is eqv 512 in A0 reading  
    // multiply 2 to get actual voltage
    AMPLITUDE=4.911*2*Vsample/1000; 
    
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
  /*
  
  ///old code for buttons with software debounce///
  if(!digitalRead(buttonDWN)){
    delay(10);
    if(!digitalRead(buttonDWN)){
    screen++;
    if(screen > 2){
      screen=0 ;   
      lcd.clear();
    }
  }
  }
  
  if(!digitalRead(buttonUP)){
    delay(10);
    if(!digitalRead(buttonUP)){
    screen--;
    if(screen<0){
      screen=2 ;
      lcd.clear();  
    }
  }
  }
  */
}//end
  //####################################################################################################



