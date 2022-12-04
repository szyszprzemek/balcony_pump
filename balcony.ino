/* BALCONY PUMP CONTROLLER
 * AUTHOR: P. Szyszkiewicz
 * DATE: 16.10.2022
 * DESC: Program start pump if the level sensor indicates presence of the water. 
 * The level threshold can be adjusted by buttons up and down.
 * Printing on LCD ans Serial all sensor inromation.
 */
 
/* include the library code: */
#include <LiquidCrystal.h>
#include "DHT.h"
#include <EEPROM.h>

/* PINs for sensors and actuators */
#define PIN_LED_ARDUINO 13
#define PIN_PUMP_RELAY  10
#define PIN_LCD_RS      12
#define PIN_LCD_EN      11
#define PIN_LCD_D4      5
#define PIN_LCD_D5      4
#define PIN_LCD_D6      3
#define PIN_LCD_D7      2
#define PIN_DHT         9     
#define PIN_WATER_LVL   A0
#define PIN_BUTTON_UP   7
#define PIN_BUTTON_DOWN 8

/* Globals */
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
DHT dht(PIN_DHT, DHT11);
bool relayStatus = false;
int thresholdWaterLvl;
int addressEEPROM = 0;

/* Lcd info */
void printLcdInfo(int waterSensorLvl, float humidity, float temperature)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tmp Hum Thr Lvl");
  lcd.setCursor(0, 1);
  lcd.print(String(int(temperature)) + "  " + String(int(humidity)) + "  " + String(int(thresholdWaterLvl)) + " " + String(int(waterSensorLvl)));
}

/* Serial info */
void printSerialInfo(int waterSensorLvl, float humidity, float temperature, float heatIndex)
{
  Serial.print("Value of the water level: ");
  Serial.print(waterSensorLvl);
  Serial.print(", Threshold water level: ");
  Serial.print(thresholdWaterLvl);
  Serial.print(", Relays Status: ");
  Serial.print(relayStatus);
  Serial.print(", Humidity: ");
  Serial.print(humidity);
  Serial.print("%,  Temperature: ");
  Serial.print(temperature);
  Serial.print("F,  Heat index: ");
  Serial.print(heatIndex);
  Serial.println("");
}

void setup() 
{
  /* Serial */
  Serial.begin(9600);

  /* LCD */
  lcd.begin(16, 2);

  /* DHT */
  dht.begin();
  
  /* Activate sensors PINs */
  pinMode(PIN_LED_ARDUINO, OUTPUT);
  pinMode(PIN_PUMP_RELAY, OUTPUT);
  pinMode(PIN_BUTTON_UP, INPUT);
  pinMode(PIN_BUTTON_DOWN, INPUT);

  /* Take threshold saved in memory */
  EEPROM.get(addressEEPROM, thresholdWaterLvl);
}

// the loop function runs over and over again forever
void loop() 
{
  /* Sensor readings */
  int waterSensorLvl = analogRead(PIN_WATER_LVL);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  int buttonUp = digitalRead(PIN_BUTTON_UP);
  int buttonDown = digitalRead(PIN_BUTTON_DOWN);
  
  /* Buttons change threshold */
  if (buttonUp == HIGH) 
  {
    thresholdWaterLvl += 1;
    EEPROM.put(addressEEPROM, thresholdWaterLvl);
  }
  if (buttonDown == HIGH) 
  {
    thresholdWaterLvl -= 1;
    EEPROM.put(addressEEPROM, thresholdWaterLvl);
  }
  
  /* Relay Controller */
  if (waterSensorLvl > thresholdWaterLvl)
  {
    relayStatus = true;
    digitalWrite(PIN_PUMP_RELAY, HIGH);
  }
  else
  {
    relayStatus = false;
    digitalWrite(PIN_PUMP_RELAY, LOW);
  }

  /* Info update */
  printSerialInfo(waterSensorLvl, humidity, temperature, heatIndex);
  printLcdInfo(waterSensorLvl, humidity, temperature);
}
