  /*
  #####################################################################################
  #
  #  	File:               ARDUINO_FUNCTION_GENERATOR_R1.INO
  #     Processor:          Arduino UNO, MEGA ou Teensy++ 2.0      
  #  	Language:	    Wiring / C /Processing /Fritzing / Arduino IDE          
  #						
  #	Objectives:         Function Generator using XR-2206
  #										  
  #	Behavior:	    This FG will generate 3 wave forms like Sine, triangle and
  #                         square. It has DC, AC and TTL output. 
  #                         Frequency (coarse and fine) adjust.
  #                         Amplitude and OffSet adjust.
  #                         Range and wave form selection by a rotary switch
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




  // libraries declaration
  #include <Wire.h>
  #include "Arduino.h"
  #include <LiquidCrystal_I2C.h> // I2C LCD display
  //#include <LiquidCrystal.h>  // 16 pins LCD display
  #include <FreqMeasure.h>


  // I2C LCD object declaration
  LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C LCD display
  //LiquidCrystal lcd(11, 12, 5, 4, 3, 2);  // 16 pins LCD display


  // variables definition
  float Vsample=0;      // for amplitude
  float AMPLITUDE=0.0;  // amplitude final value calculation
  int VsensorPin = 0;    // arduino A0 pin voltage divider sensor
  double sum=0;
  int count=0;
  float frequency=0;  // for frequency
  float period=0;     // for period
  int frequencyRange = 0;
  int wave_Form = 0;
  // arduino pins
  int sineWave = 2;          // sine wave signal from SW2C
  int triangleWave = 3;      // triangle wave signal from SW2C
  int squareWave = 4;        // square wave signal from SW2C
  int range_1 = 5;           // range 0 -100Hz
  int range_2 = 6;           // range 100Hz -1KHz
  int range_3 = 7;           // range 1KHz -10KHz
  int range_4 = 9;           // range 10KHz -100KHz
  int range_5 = 10;          // range 100KHz -1MHz


  // system messages
  const char *string_table[] =
  {   
    "       Welcome!",
    "  Arduino By Myself",
    " Function Generator",
    " Initializing, wait!",
    "",
    "",
    "",
    "",
    ""
  };


  // new character definition
  byte squareWave_1[8] = {
    B11111,
    B10001,
    B10001,
    B10001,
    B00001,
    B00001,
    B00001,
    B00001
  };
  byte squareWave_2[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00001,
    B00001,
    B00001,
    B11111
  };
  byte sineWave_1[8] = {
    B00100,
    B01010,
    B10001,
    B10001,
    B00000,
    B00000,
    B00000,
    B00000
  };
  byte sineWave_2[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B10001,
    B10001,
    B01010,
    B00100
  };
  byte triangleWave_1[8] = {
    B00100,
    B01010,
    B10001,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000
   };
   byte triangleWave_2[8] = {
    B00000,
    B00000,
    B00000,
    B10000,
    B01000,
    B00100,
    B00010,
    B00001
  };
  byte triangleWave_3[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00100,
    B01000,
    B10000,
    B00000
  };
  void setup() {
    
    // LCD initialization
    lcd.init();          // I2C LCD initialization
    //lcd.begin(20,4);     // LCD 16 pins initialization
    lcd.backlight();     // with Backlight
    lcd.clear();         // clearscreen
    
    //FreqMeasure initialization
    FreqMeasure.begin();
  
    // Wire initialization
    Wire.begin();
  
    // serial initialization
    Serial.begin(115200);
  
    
    
    // New character registration
    lcd.createChar(0, sineWave_1);
    lcd.createChar(1, sineWave_2); 
    lcd.createChar(2, triangleWave_1); 
    lcd.createChar(3, triangleWave_2); 
    lcd.createChar(4, triangleWave_3);
    lcd.createChar(5, squareWave_1);  
    lcd.createChar(6, squareWave_2);  
  
    // arduino pins definition
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

  
    // initial messages on display
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[0]);
    lcd.setCursor(0,1);
    lcd.print(string_table[1]);
    lcd.setCursor(0,2);
    lcd.print(string_table[2]);
    delay(5000);
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(string_table[3]);
    delay(5000);
    lcd.clear();         // clearscreen
  }

  void loop() {
    //lcd.clear();         // clearscreen
    // main program, call functions
    
    range();  // check the range used from the switch S1B
    displayRange();
    waveForm();  // check the wave form used from teh switch S2C
    displayWaveForm();
    amplitudeMeter(); // calculate the amplitude from the resistor devider from the DC_output signal
    //displayAmplitude();
    for(int i=0;i<100;i++){
    frequencyMeter();  // get the frequency from the TTL_output signal
    }
    displayFrequency();
    
  }



  void range(){
    // check the range used from the switch S1B
    if(!digitalRead(range_1)){
      frequencyRange = 1;
    }
    if(!digitalRead(range_2)){
      frequencyRange = 2;
    }
    if(!digitalRead(range_3)){
      frequencyRange = 3;
    }
    if(!digitalRead(range_4)){
      frequencyRange = 4;
    }
    if(!digitalRead(range_5)){
      frequencyRange = 5;
    }
  }
  
  void displayRange(){
    lcd.setCursor(0,0);
    switch(frequencyRange){
      case 1:
        lcd.print("Range: 0 - 100HZ");
        break;
      case 2:
        lcd.print("Range: 100Hz - 1KHZ");
        break;
      case 3:
        lcd.print("Range: 1KHZ - 10KHZ");
        break;
      case 4:
        lcd.print("Range: 10KHZ - 100KHZ");
        break;
      case 5:
        lcd.print("Range: 100KHZ - 1MHZ");
        break;
      default:
        break;
    }
  }



  void waveForm(){
    // check the wave form used from teh switch S2C
    if(!digitalRead(sineWave)){
      wave_Form = 1;
    }
    if(!digitalRead(triangleWave)){
      wave_Form = 2;
    }
    if(!digitalRead(squareWave)){
      wave_Form = 3;
    }
  }
  
  void displayWaveForm(){
    lcd.setCursor(0,1);
    switch(wave_Form){
      case 1:
        lcd.print("WaveForm: Sin ");
        //lcd.write(byte(0)); // for 0 char needs the byte() statement
        //lcd.write(1); // for the others chars do not need the byte()statement
        break;
      case 2:
        lcd.print("WaveForm: Tri ");
        //lcd.write(2);
        //lcd.write(3);
        //lcd.write(4);
        break;
      case 3:
        lcd.print("WaveForm: Sqr ");
        //lcd.write(5);
        //lcd.write(6);
        break;
      default:
        break;
    }
  }
  
  
  
  
  
  void amplitudeMeter() {
    //long milisec = millis(); // calculate time in milisec
    //long time=milisec/1000; // convert time to sec

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// taking 150 samples from sensors with a inerval of 2sec and then average the samples data collected
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    for(int i=0;i<150;i++){
      Vsample+=analogRead(VsensorPin);  //read the voltage from the sensor
      delay(1); // 150 samples on 300ms
    }
    Vsample=Vsample/150; 
   
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// voltage calculation
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    AMPLITUDE=4.911*2*Vsample/1000; //  callibration // 4.911 from voltage div is eqv 512 in A0 reading  // multiply 2 to get actual voltage
    
    lcd.setCursor(0,2);
    lcd.print("Amplitude: ");
    lcd.print(AMPLITUDE);
    lcd.println(" V   ");

  }

  
  void displayAmplitude(){
    lcd.setCursor(0,2);
    lcd.print("Amplitude: ");
    lcd.print(AMPLITUDE);
    lcd.println(" V   ");
  }
  
  void frequencyMeter(){
    // get the frequency from the TTL_output signal
    if (FreqMeasure.available()) {
      // average several reading together
      sum = sum + FreqMeasure.read();
      count = count + 1;
      if (count > 100) {
        frequency = FreqMeasure.countToFrequency(sum / count);
        period = 1/frequency*1000000;  // period in us
        //Serial.print(frequency); // frequency in Hz
        //Serial.print(" Hz    ");
        //Serial.print(period); //period in us
        //Serial.println(" us");
        sum = 0;
        count = 0;
      }
    }
    //delay(1);
  }
  
  void displayFrequency(){
    lcd.setCursor(0,3);
    lcd.print("Frequency: ");
    lcd.print(frequency);
    //lcd.print(" Hz");
  }
  
  void displayAll(){
    //lcd.clear();
    displayRange();
    displayWaveForm();
    displayAmplitude();
    displayFrequency();
    //delay(2);
  }
  
  
  
  
  
  


