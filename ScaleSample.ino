#include <Q2HX711.h>
#include<LiquidCrystal_I2C.h>
#include<EEPROM.h>
#include<SoftwareSerial.h>

const byte hx711_data_pin = 3; 
const byte hx711_clock_pin = 4;

Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);
LiquidCrystal_I2C lcd(0x27,A4, A5);  // set the LCD address to 0x27 for a 16 chars and 2 line display SDA, SCL
double zeroVal, activeVal, calVal, katsayi, maxVal, calKatsayi; 
int calButton = 12, zeroButton = 11, cihazId = 1, buzzer = 10;
SoftwareSerial  mySerial(A1, A2);  // RX , TX Densi Seri port modülde (1 Power +) - (2 Power -) - (3 A1 nolu pin) -(4 A2 nolu pin) 
boolean stringComplete = false;  // whether the string is complete
String sending, inputString = ""; ;

void setup() {
  mySerial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(calButton, INPUT_PULLUP);
  pinMode(zeroButton, INPUT_PULLUP);
  pinMode(buzzer, INPUT_PULLUP);
  katsayi = EEPROM_readDouble(0);
  zeroVal = EEPROM_readDouble(10);
}

void loop() {  
  lcd.setCursor(0,0);
  //lcd.print(hx711.read()/100);
  activeVal = (hx711.read()/100 - zeroVal) / katsayi;
  lcd.print(activeVal,1); lcd.print(" Kg        ");
  delay(200);
  serialFunc();
  
  ///////////////////////Cal buton okuma///////////////////
  if(digitalRead(calButton) == LOW)
  {  
    lcd.setCursor(0,1);
    lcd.print("CAL MODE        ");
    digitalWrite(buzzer, LOW);  // Kalibrasyon moduna giriildiğinde öt
    delay(500);                 // Kalibrasyon moduna giriildiğinde öt
    digitalWrite(buzzer, HIGH); // Kalibrasyon moduna giriildiğinde öt
    zeroVal = hx711.read()/100;
    delay(3000);

    digitalWrite(buzzer, LOW);  // Sıfır seviyesi okunduğunda öt
    delay(1000);                // Sıfır seviyesi okunduğunda öt
    digitalWrite(buzzer, HIGH); // Sıfır seviyesi okunduğunda öt
    lcd.setCursor(0,1);
    lcd.print("Load 25 KG   ");
    delay(10000); //Ağırlığın koyulması için beklenen süre
    calVal= hx711.read()/100;
    calKatsayi = (calVal-zeroVal) /25; //100 gram yüklendiği için elde edilen değer 100 e bölünüyor
    EEPROM_writeDouble(0, calKatsayi);
    EEPROM_writeDouble(10, zeroVal);
    digitalWrite(buzzer, LOW);  // Kalibrasyon bittiğinde öt
    delay(2000);                // Kalibrasyon bittiğinde öt
    digitalWrite(buzzer, HIGH); // Kalibrasyon bittiğinde öt
    lcd.clear();
  }
  /////////////////////////////////////////////////////////


  if(digitalRead(zeroButton) == LOW)
  {
    zeroVal = hx711.read()/100;
  }
}

void serialFunc()
{
  if(stringComplete && sending == "001R")
    {
      mySerial.print(activeVal,1);
      inputString = "";
      sending ="";
      stringComplete = false;
    }
    
  while(mySerial.available())
     {
        char inChar = (char)mySerial.read();  // add it to the inputString:      
        inputString += inChar;
        if (inChar == '\n') {
          sending = inputString;
          stringComplete = true;
      }
    }  
}

void EEPROM_writeDouble(int ee, double value) // Litre fiyatı için belirlenen doble değeri EEprom a yazan fonksiyon
{
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
}

double EEPROM_readDouble(int ee) // Litre fiyatı için belirlenen doble değeri EEprom dan okuyan fonksiyon
{
   double value = 0.0;
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       *p++ = EEPROM.read(ee++);
   return value;
}
