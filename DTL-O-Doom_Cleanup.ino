#include <Wire.h> 
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>


//defines for count down clock
#define YEAR  2042 // Date and time of some future event
#define MONTH    8 // (like retirement @ 67)
#define DAY      9 // But here it was set for Halley's comet
#define HOUR    16 // Example time here is set for 4PM,
#define MINUTE   1 // DON'T use leading zero or value is octal!

//initialize the DHT12 Temp/%RH sensor
const int DHT12 = 0x5C;     //datasheet says 0xB8 but I saw a write up that says they saw 0x5C when scanning the I2C bus
char DHT12Data[5];    //setup array for data read from DHT12
float DHT12_Humidity = 0;
float DHT12_Temp = 0;
float TempInF = 0;

LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display

const int ResetButton = 2;
int tick = 0;
int years = 0;
int months = 0;
int days = 0;
int hours = 0; // start hours
int minutes = 0; //start min
int seconds = 90; //start seconds
int ThisMonth = 0;
int DaysThisMonth = 0;  //used to calculate # of days this month

int DelayTime = 0;

int ClearDisplay = 0;

// initialize the RTC library and setup calendar
RTC_DS1307 RTC;

// Number of days since 2000/01/01, valid for 2000-2099.  Essentially a
// copy of the date2days() function in RTClib, but it's been declared static
// I am too lazy to set it as global
uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) 
{
  static const uint8_t daysInMonth[] PROGMEM =
    { 31,28,31,30,31,30,31,31,30,31,30,31 };
  if(y >= 2000) y -= 2000;
  uint16_t days = d;
  for(uint8_t i=1; i<m; ++i) days += pgm_read_byte(daysInMonth + i - 1);
  if((m > 2) && (!(y & 3))) days++;        // Leap day for current year
  return days + 365 * y + (y + 3) / 4 - 1; // Add leap days for prior years
}//end uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) 
 
void setup() 
{
    Wire.begin();         //setup I2C
  // initialize the lcd 
  lcd.init();                      
  lcd.backlight();
  lcd.print("!! Retirement  Timer !!");

  Serial.begin(57600);  //setup the serial port for debug

  RTC.begin();          //setup RTC
  
  pinMode(ResetButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ResetButton), ResetTimer, LOW);

  //RTC.adjust(DateTime(F(__DATE__), F(__TIME__))); //-->with battery installed this should not be necessary

  DelayTime = 1000; 

}//end void setup()

void loop() 
{
  lcd.clear();
//  lcd.print("Count Down Timer ");
  delay(250);
  
  while (hours > 0 || minutes > 0 || seconds >= 0) 
  {
    if (ClearDisplay == 1)
    {
      lcd.clear();
      lcd.noAutoscroll();
      ClearDisplay = 0;
    }
    GetTime();
    GetEndTime();
    GetDHT12Data();
    delay(DelayTime);
 }//end   while (hours > 0 || minutes > 0 || seconds >= 0)
 
}//end void loop()

//take care not to put too much into this FTN called by the interupt

void ResetTimer()
{
   lcd.clear();
   ClearDisplay = 1;
   DelayTime = 1000;
}// end void ResetTimer()



