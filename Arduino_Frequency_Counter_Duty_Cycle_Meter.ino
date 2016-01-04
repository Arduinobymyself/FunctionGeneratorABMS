//Arduino frequency counter/duty cycle meter
//www.electronicsblog.net/
// OK testado arduinobymyself
//using interrupt 0 from arduino pin 2
//up to 40KHz

#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>

//LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal lcd(12, 11, 10, 7, 6, 4, 3);

#define Button 9

int divider[6] ={
  0,1,8,64,256,1024};

int prescaler=5;

int b=0;

int screen =0;

double count =0;
double middle =0;
double usage =0;
char x=0;

ISR(TIMER1_OVF_vect) {

  if (prescaler<4) {
    prescaler++;
  }

}
void interrupt()
{
  if (!x) {
    count=TCNT1;
    TCNT1=0x000;  
    TCCR1B=prescaler;
    attachInterrupt(0, interrupt, FALLING);

  }

  else {
    middle=TCNT1;
    attachInterrupt(0, interrupt, RISING);

  }

  x=~x; 
}

void setup()   { 

  //lcd.init();
  //lcd.backlight();
  lcd.begin(16, 2);

  pinMode(Button, INPUT);
  digitalWrite(Button, HIGH); //pull up resistor

  TIMSK1=0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  attachInterrupt(0, interrupt, RISING);
  
  //initial mesages
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" ArduinoByMyself");
  lcd.setCursor(0,1);
  lcd.print(" Frequency meter");
  delay(5000);

}

void loop()
{ 
/// screen modes
  switch (screen) {

  case 0: 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("Freq ");
    lcd.print((16000000.0/divider[prescaler]/count),1);
    lcd.print(" Hz ");
    lcd.setCursor(0,1); 
    lcd.print("Duty ");  
    lcd.print((middle/count*100),1); 
    lcd.print(" %  ");
    lcd.print(""); 
    break;

  case 1:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Period: ");
    lcd.print((0.0000625*divider[prescaler]*count),2);
    lcd.print(" ms     ");
    break;   

  case 2:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("H ");
    lcd.print(0.0000625*divider[prescaler]*middle);
    lcd.print(" ms    ");
    lcd.setCursor(0,1);
    lcd.print("L ");
    lcd.print(0.0000625*divider[prescaler]*(count-middle));
    lcd.print(" ms    ");
    break;   

  case 3:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Prescaler /");
    lcd.print(divider[prescaler]);
    lcd.setCursor(0,1);
    lcd.print("Count.use ");
    usage=count/65536*100;
    lcd.print(usage);
    lcd.print("%");
    break; 

  }

  delay(1000);

  if (prescaler>1) {

    if (usage<0.15) {
      prescaler--; 
      delay(200);
    } 

  }    

  ///button /////////

  if (!digitalRead(Button)&&!b) {

    screen++;
    if (screen==4) {
      screen=0 ;   
    }
    lcd.clear();

    b=3;

  };

  if (!b==0) b--;

}
