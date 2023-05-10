#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#define FIREBASE_AUTH "hpbGlvLmHegIAuDfeCmDF2I8havz5ux2njGQgu6x"
#define WIFI_SSID "Hieu"
#define WIFI_PASSWORD "123321111"
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#define SS 15
#define RST 16
#define DI0 4
#define API_KEY "AIzaSyBmK6XDCGRnaxtvXMvSp7P-SWmJqyEJ9u4"
#define DATABASE_URL "sp02-dfccd-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define USER_EMAIL "taiboy129@gmail.com"
#define USER_PASSWORD "1292001"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
//#define TX_P 17
#define BAND 433E6
//#define ENCRYPT 0x78
String temp;
String hum;
String lux;
String value;
int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 12; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator = 5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength = 8; // from 2 to 20, default is 8
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);


  SPI.begin();
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(txPower, 1);
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(signalBandwidth);
  LoRa.setCodingRate4(codingRateDenominator);
  LoRa.setPreambleLength(preambleLength);
}

void loop() {


  // try to parse packet
  int pos1, pos2, pos3, pos4;

  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // received a packet
    Serial.print("Received packet:  ");
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData);
    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    pos1 = LoRaData.indexOf('/');
    pos2 = LoRaData.indexOf('&');
    pos3 = LoRaData.indexOf('#');

    temp = LoRaData.substring(0, pos1);
    hum = LoRaData.substring(pos1 + 1, pos2);
    lux = LoRaData.substring(pos2 + 1, pos3);
    value = LoRaData.substring(pos3 + 1, LoRaData.length());
    Firebase.setString(fbdo, F("/Sensor/temperature"), temp);
    Firebase.setString(fbdo, F("/Sensor/humidity"), hum);
    Firebase.setString(fbdo, F("/Sensor/light"), lux);
    Firebase.setString(fbdo, F("/Sensor/moilsture"), value);
    Serial.print(F("Temperature = "));
    Serial.print(temp);
    Serial.println(F("*C"));

    Serial.print(F("Humidity = "));
    Serial.print(hum);
    Serial.println(F("%"));

    Serial.print(F("Light = "));
    Serial.print(lux);
    Serial.println(F(" lx"));

    Serial.print(F("Soil moisure = "));
    Serial.print(value);
    Serial.println(F(" %"));

    Serial.println();

  }
}
