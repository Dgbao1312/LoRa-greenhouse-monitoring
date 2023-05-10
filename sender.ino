#include <DHT.h>
#include <BH1750.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

int counter = 0;

// Parameters you can play with :

int txPower = 14; 
int spreadingFactor = 12; 
long signalBandwidth = 125E3; 
int codingRateDenominator=5; 
int preambleLength=8; 

#define SS 10
#define RST 9
#define DI0 2
#define BAND 433E6  // Here you define the frequency carrier
#define DHTPIN 5
#define DHTTYPE DHT11
#define RELAY 7
String LoRaMessage = "";

BH1750 lightMeter;
String outgoing;              // outgoing message
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  while (!Serial);
  dht.begin();
  Serial.println("LoRa Sender");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);

  SPI.begin();
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 LoRa.setTxPower(txPower,1);
 LoRa.setSpreadingFactor(spreadingFactor);
 LoRa.setSignalBandwidth(signalBandwidth);
 LoRa.setCodingRate4(codingRateDenominator);
 LoRa.setPreambleLength(preambleLength);
 //LoRa.setPolarity(1);
 //LoRa.setFSK(); 
 
}

void loop() {
    int temp = dht.readTemperature();
    Serial.print("Nhiet Do :");  
    Serial.print(temp);
    Serial.println("do c");
    delay(200);
    float hum = dht.readHumidity();
    Serial.print("Do am :");  
    Serial.print(hum);
    Serial.println(" %");
    delay(200);
    Wire.begin();
    lightMeter.begin();
    float lux = lightMeter.readLightLevel();
    Serial.print("Anh Sang: ");
    Serial.print(lux);
    Serial.println(" lx");
    int value = analogRead(A0);
    value = map(value,0,1023,100,0);
    Serial.print("do am dat: ");
    Serial.print(value);
    Serial.println(" %"); 
    if (value < 20) digitalWrite(A7, 1);
    else digitalWrite(A7, 0);
    delay(500);
    LoRaMessage = String(temp) + "/" + String(hum) + "&" + String(lux) + "#" + String(value);
    LoRa.beginPacket();
    LoRa.print(LoRaMessage);
    LoRa.endPacket();
  counter++;
  Serial.println(counter);
 

  delay(100);
}