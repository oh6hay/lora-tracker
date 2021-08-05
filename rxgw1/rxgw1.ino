#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 

#include <LoRaComms.h>

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    433E6

uint8_t delimiter[] = {0x12,0x34,0x56,0x78};

SSD1306 display(0x3c, 21, 22);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void loraData();
static byte buffer[255];

void prettilyPrint(lpkt p) {
    Serial.print("LOCATION_NOTIFY src=");
    Serial.print(p.srcaddr);
//    lposition *pos = (lposition*)(&p.payload);
//    Serial.print(" lat=");
//    Serial.print(pos->lat);
//    Serial.print(" lon=");
//    Serial.print(pos->lng);
//    Serial.print(" satellites=");
//    Serial.print(pos->satellites);
//    Serial.print(" agemillis=");
//    Serial.print(pos->agemillis);
//    Serial.print(" rssi=");
//    Serial.println(LoRa.packetRssi());
}

void onReceive(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);

  for (int i = 0; i < packetSize; i++) { buffer[i] = LoRa.read(); }
  lpkt p;
  memcpy(&p, buffer, sizeof(lpkt));
  p.rssi = (uint8_t) -LoRa.packetRssi();
  memcpy(buffer, &p, sizeof(lpkt));
  
  if (p.message_type == LOCATION_NOTIFY) {
    //prettilyPrint(p);
    // write delimiter
    Serial.write(delimiter, 4);
    Serial.write(buffer, packetSize);
    Serial.flush();
  }
  
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
}
void onTxDone() {
 display.drawString(0, 48, "tx'd");
}

void showMillis() {
  display.drawString(0, 0, "ticks:");
  display.drawString(0, 18, String(millis()));
}

void loraData(){
  display.drawString(0, 32, packet);
  //Serial.println(rssi);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();

  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high、

  display.init();
  display.flipScreenVertically();  
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0 , 0 , "Fuck off");
  display.display();

  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    display.drawString(0, 12, "Starting LoRa failed!");
    display.display();
    while (1);
  }
  LoRa.onTxDone(onTxDone);
  LoRa.onReceive(onReceive);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(10);
  LoRa.setCodingRate4(8);
  LoRa.receive();
  Serial.println("init ok");

}

void loop() {
  display.clear();
  display.drawString(0, 0, String(LoRa.rssi()));
  display.drawString(24, 0, String(LoRa.packetFrequencyError() / 1000L));
  display.drawString(46, 0, String(LoRa.available()));
//  loraData();
  display.display();

  delay(1000);
//  Serial.print("Tick ");
//  Serial.println(millis());

}
