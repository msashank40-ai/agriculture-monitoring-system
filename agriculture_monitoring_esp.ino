#include <WiFi.h>

// ======================================================
// WIFI
// ======================================================

const char* ssid = "Oneplus";
const char* password = "123456789";

// ======================================================
// UART
// ======================================================

#define RX_PIN 16
#define TX_PIN 17

HardwareSerial ArduinoSerial(1);


// ======================================================
// SETUP
// ======================================================

void setup() {

  // ESP32 Serial Monitor
  Serial.begin(115200);

  // UART from Arduino UNO
  ArduinoSerial.begin(
    9600,
    SERIAL_8N1,
    RX_PIN,
    TX_PIN
  );

  Serial.println();
  Serial.println("ESP32-S3 STARTING...");


  // ====================================================
  // CONNECT TO WIFI
  // ====================================================

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi CONNECTED!");

  Serial.print("ESP32 IP ADDRESS: ");

  Serial.println(WiFi.localIP());
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  // ====================================================
  // RECEIVE DATA FROM ARDUINO
  // ====================================================

  if (ArduinoSerial.available()) {

    String data =
      ArduinoSerial.readStringUntil('\n');

    data.trim();

    Serial.print("DATA FROM ARDUINO: ");

    Serial.println(data);
  }
}