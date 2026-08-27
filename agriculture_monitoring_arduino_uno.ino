#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ======================================================
// PIN DEFINITIONS
// ======================================================

#define DHT_PIN A1
#define DHT_TYPE DHT22

#define LDR_PIN A0
#define MOISTURE_PIN A2

#define RED_LED 3
#define GREEN_LED 4

#define BUTTON1 7
#define BUTTON2 6
#define BUTTON3 5

// ======================================================
// LCD
// ======================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ======================================================
// DHT
// ======================================================

DHT dht(DHT_PIN, DHT_TYPE);

// ======================================================
// BUTTON STATES
// ======================================================

int lastButton1 = HIGH;
int lastButton2 = HIGH;
int lastButton3 = HIGH;

int button1Mode = 0;
int button2Mode = 0;
int button3Mode = 0;

// ======================================================
// TEMPERATURE ALERT
// ======================================================

bool temperatureWasHigh = false;
bool temperatureAlert = false;

unsigned long alertStartTime = 0;

// ======================================================
// TIMERS
// ======================================================

unsigned long lastLCDUpdate = 0;
unsigned long lastUARTUpdate = 0;


// ======================================================
// SETUP
// ======================================================

void setup() {

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  // UART
  // Arduino UNO TX -> ESP32-S3 RX
  Serial.begin(9600);

  dht.begin();

  lcd.init();
  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("AGRICULTURE");

  lcd.setCursor(0, 1);
  lcd.print("MONITOR SYSTEM");

  delay(2000);

  lcd.clear();
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  // ====================================================
  // READ SENSORS
  // ====================================================

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int lightRaw = analogRead(LDR_PIN);
  int moistureRaw = analogRead(MOISTURE_PIN);


  // ====================================================
  // LIGHT %
  // ====================================================

  int lightPercent = map(
    lightRaw,
    0,
    1023,
    0,
    100
  );

  lightPercent = constrain(
    lightPercent,
    0,
    100
  );


  // ====================================================
  // SOIL MOISTURE %
  // ====================================================

  int moisturePercent = map(
    moistureRaw,
    1023,
    0,
    0,
    100
  );

  moisturePercent = constrain(
    moisturePercent,
    0,
    100
  );


  // ====================================================
  // PLANT CONDITION
  // ====================================================

  String plantCondition;

  if (isnan(temperature) || isnan(humidity)) {

    plantCondition = "BAD";

  }
  else {

    int goodParameters = 0;
    int averageParameters = 0;


    // TEMPERATURE

    if (temperature >= 18 && temperature <= 30) {

      goodParameters++;

    }
    else if (temperature >= 15 && temperature < 18) {

      averageParameters++;

    }
    else if (temperature > 30 && temperature <= 35) {

      averageParameters++;

    }


    // HUMIDITY

    if (humidity >= 40 && humidity <= 70) {

      goodParameters++;

    }
    else if (humidity >= 30 && humidity < 40) {

      averageParameters++;

    }
    else if (humidity > 70 && humidity <= 80) {

      averageParameters++;

    }


    // MOISTURE

    if (moisturePercent >= 40 &&
        moisturePercent <= 75) {

      goodParameters++;

    }
    else if (moisturePercent >= 25 &&
             moisturePercent < 40) {

      averageParameters++;

    }
    else if (moisturePercent > 75 &&
             moisturePercent <= 85) {

      averageParameters++;

    }


    // LIGHT

    if (lightPercent >= 40 &&
        lightPercent <= 80) {

      goodParameters++;

    }
    else if (lightPercent >= 25 &&
             lightPercent < 40) {

      averageParameters++;

    }
    else if (lightPercent > 80 &&
             lightPercent <= 90) {

      averageParameters++;

    }


    // FINAL CONDITION

    if (goodParameters >= 3) {

      plantCondition = "GOOD";

    }
    else if ((goodParameters + averageParameters) >= 3) {

      plantCondition = "AVG";

    }
    else {

      plantCondition = "BAD";
    }
  }


  // ====================================================
  // TEMPERATURE ALERT
  // ====================================================

  if (!isnan(temperature)) {

    if (temperature > 30 &&
        !temperatureWasHigh) {

      temperatureWasHigh = true;

      temperatureAlert = true;

      alertStartTime = millis();

      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }


    // RED LED OFF AFTER 5 SECONDS

    if (temperatureAlert &&
        millis() - alertStartTime >= 5000) {

      temperatureAlert = false;

      digitalWrite(RED_LED, LOW);
    }


    // RESET ALERT

    if (temperature <= 30) {

      temperatureWasHigh = false;

      temperatureAlert = false;

      digitalWrite(RED_LED, LOW);
    }
  }


  // ====================================================
  // GREEN LED
  // ====================================================

  if (!temperatureAlert) {

    if (plantCondition == "GOOD") {

      digitalWrite(GREEN_LED, HIGH);

    }
    else {

      digitalWrite(GREEN_LED, LOW);
    }
  }


  // ====================================================
  // BUTTON 1 - MOISTURE
  // ====================================================

  int currentButton1 = digitalRead(BUTTON1);

  if (currentButton1 == LOW &&
      lastButton1 == HIGH) {

    button1Mode++;

    if (button1Mode > 1) {

      button1Mode = 0;
    }

    button2Mode = 0;
    button3Mode = 0;

    lcd.clear();

    delay(200);
  }

  lastButton1 = currentButton1;


  // ====================================================
  // BUTTON 2 - TEMPERATURE / HUMIDITY
  // ====================================================

  int currentButton2 = digitalRead(BUTTON2);

  if (currentButton2 == LOW &&
      lastButton2 == HIGH) {

    button2Mode++;

    if (button2Mode > 2) {

      button2Mode = 0;
    }

    button1Mode = 0;
    button3Mode = 0;

    lcd.clear();

    delay(200);
  }

  lastButton2 = currentButton2;


  // ====================================================
  // BUTTON 3 - LIGHT / CONDITION / NPK
  // ====================================================

  int currentButton3 = digitalRead(BUTTON3);

  if (currentButton3 == LOW &&
      lastButton3 == HIGH) {

    button3Mode++;

    if (button3Mode > 3) {

      button3Mode = 0;
    }

    button1Mode = 0;
    button2Mode = 0;

    lcd.clear();

    delay(200);
  }

  lastButton3 = currentButton3;


  // ====================================================
  // LCD UPDATE
  // ====================================================

  if (millis() - lastLCDUpdate >= 1000) {

    lastLCDUpdate = millis();

    lcd.clear();


    // NORMAL SCREEN

    if (button1Mode == 0 &&
        button2Mode == 0 &&
        button3Mode == 0) {

      lcd.setCursor(0, 0);

      lcd.print("PLANT: ");
      lcd.print(plantCondition);

      lcd.setCursor(0, 1);

      lcd.print("L:");
      lcd.print(lightPercent);

      lcd.print("% M:");
      lcd.print(moisturePercent);

      lcd.print("%");
    }


    // MOISTURE

    if (button1Mode == 1) {

      lcd.setCursor(0, 0);

      lcd.print("SOIL MOISTURE");

      lcd.setCursor(0, 1);

      lcd.print(moisturePercent);
      lcd.print("%");
    }


    // TEMPERATURE

    if (button2Mode == 1) {

      lcd.setCursor(0, 0);

      lcd.print("TEMPERATURE");

      lcd.setCursor(0, 1);

      if (isnan(temperature)) {

        lcd.print("ERROR");

      }
      else {

        lcd.print(temperature, 1);

        lcd.print((char)223);

        lcd.print("C");
      }
    }


    // HUMIDITY

    if (button2Mode == 2) {

      lcd.setCursor(0, 0);

      lcd.print("HUMIDITY");

      lcd.setCursor(0, 1);

      if (isnan(humidity)) {

        lcd.print("ERROR");

      }
      else {

        lcd.print(humidity, 1);

        lcd.print("%");
      }
    }


    // LIGHT

    if (button3Mode == 1) {

      lcd.setCursor(0, 0);

      lcd.print("LIGHT INTENSITY");

      lcd.setCursor(0, 1);

      lcd.print(lightPercent);

      lcd.print("%");
    }


    // PLANT CONDITION

    if (button3Mode == 2) {

      lcd.setCursor(0, 0);

      lcd.print("PLANT CONDITION");

      lcd.setCursor(0, 1);

      if (plantCondition == "GOOD") {

        lcd.print("Optimal");

      }
      else if (plantCondition == "AVG") {

        lcd.print("MODERATE");

      }
      else {

        lcd.print("POOR");
      }
    }


    // APPROXIMATE NPK

    if (button3Mode == 3) {

      int nitrogen;
      int phosphorus;
      int potassium;


      if (plantCondition == "GOOD") {

        nitrogen = 60;
        phosphorus = 40;
        potassium = 55;

      }
      else if (plantCondition == "AVG") {

        nitrogen = 40;
        phosphorus = 25;
        potassium = 40;

      }
      else {

        nitrogen = 20;
        phosphorus = 15;
        potassium = 20;
      }


      lcd.setCursor(0, 0);

      lcd.print("N:");
      lcd.print(nitrogen);

      lcd.print(" P:");
      lcd.print(phosphorus);


      lcd.setCursor(0, 1);

      lcd.print("K:");
      lcd.print(potassium);

      lcd.print(" mg/kg*");
    }
  }


  // ====================================================
  // SEND DATA TO ESP32-S3
  // ====================================================

  if (millis() - lastUARTUpdate >= 2000) {

    lastUARTUpdate = millis();

    /*
      UART FORMAT:

      DATA,TEMPERATURE,HUMIDITY,LIGHT,MOISTURE,CONDITION

      Example:

      DATA,27.5,65.2,72,58,GOOD
    */

    Serial.print("DATA,");


    // TEMPERATURE

    if (isnan(temperature)) {

      Serial.print("0");

    }
    else {

      Serial.print(
        temperature,
        1
      );
    }


    Serial.print(",");


    // HUMIDITY

    if (isnan(humidity)) {

      Serial.print("0");

    }
    else {

      Serial.print(
        humidity,
        1
      );
    }


    Serial.print(",");


    // LIGHT

    Serial.print(lightPercent);

    Serial.print(",");


    // MOISTURE

    Serial.print(moisturePercent);

    Serial.print(",");


    // CONDITION

    Serial.println(plantCondition);
  }


  delay(50);
}