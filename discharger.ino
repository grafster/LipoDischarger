/* 
* Battery Capacity Checker
* Uses Nokia 5110 Display
* Uses 1 Ohm power resister as shunt - Load can be any suitable resister or lamp
* 
* YouTube Video: https://www.youtube.com/embed/qtws6VSIoYk
* 
* http://AdamWelch.Uk
* 
* Required Library - LCD5110_Graph.h - http://www.rinkydinkelectronics.com/library.php?id=47
*/
 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
 
#define gatePin 10
#define highPin A0  
#define lowPin A1

#define CellCount 6
#define RealCellCount 2

int gatePins [] = {8,9};
int highPins [] = {A3, A5};
int lowPins [] = {A4, A6};

boolean finished[] = {false, false, false, false, false, false};


 
//boolean finished = false;
int printStart = 0;
int interval = 5000;  //Interval (ms) between measurements
 
float mAh []  = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float shuntRes = 1.0;  // In Ohms - Shunt resistor resistance
float voltRef = 5; // Reference voltage (probe your 5V pin) 
float current = 0.0;
float battVolt = 0.0;
float shuntVolt = 0.0;
float battLow = 2.9;
 
unsigned long previousMillis[] = {0,0,0,0,0,0};
unsigned long millisPassed = 0;
 
void setup() {
 
  Serial.begin(9600);
  Serial.println("Battery Capacity Checker v1.1");
  Serial.println("battVolt   current     mAh");

  for (int i = 0; i < RealCellCount; i++)
  {
    pinMode(gatePins[i], OUTPUT);
    digitalWrite(gatePins[i], LOW);
  }
  lcd.begin(20,4); // Init LCD
  lcd.setCursor (0,0);
  lcd.print("Battery");
  lcd.setCursor (0,1);
  lcd.print("Check");
  lcd.setCursor (0,2);
  lcd.print("Please Wait");
  lcd.setCursor (0,3);
  lcd.print("AdamWelch.Uk");
  
 
  
  delay(2000);

  lcd.setCursor (0,0);
  lcd.print ("                    ");
  lcd.setCursor (0,1);
  lcd.print ("V                   ");
  lcd.setCursor (0,2);
  lcd.print ("A                   ");
  lcd.setCursor (0,3);
  lcd.print ("mAh                 ");
  
}
 
void loop() {

  // For each cell
  for (int i = 0; i < CellCount; i++)
  {
    if (i % 3 == 0) 
    {
       delay(interval);
       
    }
    if (i < RealCellCount)
    {
      // Read the voltages
      float readValue = analogRead(highPins[i]);
      Serial.print ("Read from high pin " + String(i) + " Value " + String(readValue, 2) + "\r\n");
      battVolt = readValue * voltRef / 1024.0;
      shuntVolt = analogRead(lowPins[i]) * voltRef / 1024.0;
    }
    else
    {
      battVolt = 9.6;
      shuntVolt = 6.2;
    }

    // Calculate where we should be displaying this on the LCD
    int horizontalLCDPos = (  (i % 3)  *5)+5; // % characters per cell, + 5 for the row headers
    
    lcd.setCursor(horizontalLCDPos, 0);
    lcd.print("     "); // Clear the space

    lcd.setCursor(horizontalLCDPos, 0);
    lcd.print(String(i+1)); // Output the battery number + 1 to start counting from 1 like normal people do
    
    if (finished[i])
    {
       lcd.print("F");
    }

    //battVolt = 8.62;

   // If we're still going, because the voltage is above the minimum, and we haven't already set finished to true
  if(battVolt >= battLow && finished[i] == false)
  {
      digitalWrite(gatePins[i], HIGH);
      unsigned long now = millis();
      millisPassed = now - previousMillis[i];
      current = (battVolt - shuntVolt) / shuntRes;
      mAh[i] = mAh[i] + (current * 1000.0) * (millisPassed / 3600000.0);
      previousMillis[i] = now;

      
      lcd.setCursor(horizontalLCDPos, 1);
      lcd.print("     "); // Clear the space
      lcd.setCursor(horizontalLCDPos ,1);
      lcd.print(String(battVolt, 2));
      
      lcd.setCursor (horizontalLCDPos ,2);
      lcd.print("     ");
      lcd.setCursor (horizontalLCDPos ,2);
      lcd.print(String(current, 2));
      
      lcd.setCursor (horizontalLCDPos ,3);
      lcd.print("     ");
      lcd.setCursor (horizontalLCDPos ,3);
      lcd.print(String(mAh[i], 0));

 /*
      myGLCD.clrScr();
      myGLCD.print("Discharge",CENTER,0);
      myGLCD.print("Voltage:",0,10);
      myGLCD.printNumF(battVolt, 2,50,10);
      myGLCD.print("v",77,10);
      myGLCD.print("Current:",0,20);
      myGLCD.printNumF(current, 2,50,20);
      myGLCD.print("a",77,20);
      myGLCD.printNumI(mAh,30,30);
      myGLCD.print("mAh",65,30);
      myGLCD.print("Running",CENTER,40);
      myGLCD.update(); 
   */    
      Serial.print(battVolt);
      Serial.print("\t");
      Serial.print(current);
      Serial.print("\t");
      Serial.println(mAh[i]);
       
  }
  if(battVolt < battLow && !finished[i])
  {
      digitalWrite(gatePins[i], LOW);
      
      finished[i] = true;
      /* 
      if(mAh < 10) {
        printStart = 40;
      }
      else if(mAh < 100) {
        printStart = 30;
      }
      else if(mAh <1000) {
        printStart = 24;
      }
      else if(mAh <10000) {
        printStart = 14;
      }
      else {
        printStart = 0;
      }*/
       /*
      myGLCD.clrScr();
      myGLCD.print("Discharge",CENTER,0);
      myGLCD.print("Voltage:",0,10);
      myGLCD.printNumF(battVolt, 2,50,10);
      myGLCD.print("v",77,10);
      myGLCD.setFont(MediumNumbers);
      myGLCD.printNumI(mAh,printStart,21);
      myGLCD.setFont(SmallFont);
      myGLCD.print("mAh",65,30);
      myGLCD.print("Complete",CENTER,40);
      myGLCD.update(); 
       */
      lcd.setCursor (horizontalLCDPos ,1);
      lcd.print(String(battVolt, 2));
      
      lcd.setCursor (horizontalLCDPos ,2);
      lcd.print(String(current, 2));
      lcd.setCursor (horizontalLCDPos ,3);
      lcd.print(String(mAh[i],0));      

  }
  else if (finished[i])
  {
      lcd.setCursor (horizontalLCDPos ,1);
      lcd.print(" --- ");
      
      lcd.setCursor (horizontalLCDPos ,2);
      lcd.print(" --- ");
      lcd.setCursor (horizontalLCDPos ,3);
      lcd.print("     ");
      lcd.setCursor (horizontalLCDPos ,3);
      lcd.print(String(mAh[i],0));      
    
  }
  
  }
  delay(interval);
}    
