#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
// Flow Sensor 3.3V

// Set the LCD address

const int I2C_ADDR = 0x27;  // Address might vary, check yours

// Set the LCD dimensions (columns x rows)
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

float Celsius = 0;
float celsius=16;

// Create an LCD object. Provide the I2C address, columns, and rows.
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

#define HzPerLiter 8.1   
#define pulse2liter 486.0 
#define interval 3000.0



// Define the pin to which the relay module is connected
const int relayPin = 3;

const byte intrPin = 2;
bool run_flag =false;
unsigned long startTime= millis();

int LperM=0;
volatile int rate_cnt=0;
unsigned long tot_cnt=0;
unsigned long tot_sec=0;
unsigned long tot_min=0;
unsigned long totLit=0;

char tbp[65];



void setup(){
  sensors.begin();
  Serial.begin(9600);
  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(intrPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(intrPin),Pulse_cnt, FALLING);
  Serial.println("Water flow = Approximate (No Exit. J-P S.)");
  sprintf(tbp, "Report every %d seconds" , round(interval/1000)); // Format comment
  Serial.println(tbp);
   Serial.println(" ");
   
    // Set the relay pin as an OUTPUT
  pinMode(relayPin, OUTPUT);

  // Initially, turn off the pump
  digitalWrite(relayPin, LOW);

}


void Pulse_cnt(){
  rate_cnt++;
  tot_cnt++;
  run_flag= true;
}

void loop(){
  
  
  while (millis() - startTime <= interval){
    digitalWrite(LED_BUILTIN, LOW);
    if (run_flag){
      digitalWrite(LED_BUILTIN, HIGH);
      run_flag= false;
    }
    
  }
  
  tot_sec += interval/1000;
  tot_min= round(tot_sec/60);
  LperM = round(rate_cnt/(HzPerLiter*(interval)/1000)); // (pulses/interval)/(486*interval)
  totLit= round(tot_cnt/pulse2liter);
  
 Serial.print(" Instanaeouse Liters/Min : ");
 Serial.println(LperM);
 lcd.setCursor(0,0);
 lcd.print("In L/M:");
 lcd.print(LperM);
 
 
 Serial.print("Total Liters since Startup :  ");
 Serial.print(totLit);
 lcd.print(" T_L:");
 lcd.print(totLit);
 
 Serial.print("Total Minutes Since Startup: ");
 Serial.println(tot_min);

 Serial.println("==================================");
 
  sensors.requestTemperatures();
  Celsius = sensors.getTempCByIndex(0);
  celsius= celsius-0.01;

if (LperM <2){
  digitalWrite(relayPin, LOW);
   Serial.println("Pmp is ON");
  Serial.print("T=");
  Serial.print(celsius);
  Serial.print("C");
  lcd.setCursor(0,1);
  lcd.print("Pmp ON,");
  lcd.print("T=");
  lcd.print(celsius);
  lcd.print("C");
  }
  
if (LperM>=3){
 
  digitalWrite(relayPin, HIGH);
  Serial.println("Pump is OFF");
  Serial.print (celsius);
  Serial.print("C");
  
  lcd.setCursor(0,1);
  lcd.print("Pmp OF,");
  lcd.print(celsius);
  lcd.print("C");
  }
  startTime= millis();
   rate_cnt =0;


}
