#include <TinyGPS++.h>
#include <axp20x.h>
#include <LoRaComms.h>

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
SSD1306 display(0x3c, 21, 22);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND  433E6


TinyGPSPlus gps; 
HardwareSerial GPS(1);
AXP20X_Class axp;

void setup()
{
  Serial.begin(115200);
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.setSignalBandwidth(7.8E3);
  LoRa.setTxPower(20);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(10);
  LoRa.setCodingRate4(8);

  //LoRa.onReceive(cbk);
//  LoRa.receive();
  Serial.println("LoRa init ok");
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);

  
  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
    Serial.println("AXP192 Begin PASS");
  } else {
    Serial.println("AXP192 Begin FAIL");
  }
  axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
  axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
  GPS.begin(9600, SERIAL_8N1, 34, 12);   //17-TX 18-RX

  pinMode(16,OUTPUT);
  pinMode(2,OUTPUT);
  
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
   
  delay(1500);

}

static long seq = 1;
static unsigned long long lastsend;
static uint8_t buffer[255];
void maybeSendPosition() {
//  if (!gps.location.isValid()) {
//    return;
//  }
  lposition pos;
  pos.hour = gps.time.hour();
  pos.minute = gps.time.minute();
  pos.second = gps.time.second();
  pos.satellites = gps.satellites.value();
  pos.year = gps.date.year();
  pos.month = gps.date.month();
  pos.day = gps.date.day();
  pos.agemillis = gps.location.age();
  pos.lat = gps.location.lat();
  pos.lng = gps.location.lng();
  pos.kmph = gps.speed.kmph();
  pos.course = gps.course.deg();
  pos.meters = gps.altitude.meters();
  pos.padding = 0;
  if (millis() - lastsend > 6000) {
    lpkt packet;
    packet.flags = LRC_FLAG_BROADCAST;
    packet.message_type = LOCATION_NOTIFY;
    packet.srcaddr = 33;
    packet.dstaddr = 1;
    packet.fragmentno = 1;
    packet.fragments = 1;
    packet.payload_size = (uint8_t)sizeof(lposition);
    packet.padding = 0;
    packet.seqno = (uint32_t)seq++;

    Serial.print("pos size=");
    Serial.print(sizeof(lposition));
    Serial.print(" pkt size=");
    Serial.print(sizeof(lpkt));
    Serial.print(" buffer size=");
    Serial.println(sizeof(buffer));
    
//    memcpy(&(packet.payload), &pos, sizeof(pos));
//    memcpy(&buffer[0], &packet, sizeof(lpkt));
    Serial.println("begin ");
    LoRa.beginPacket();
    int payload_len = 12 + sizeof(lposition);
    int written = LoRa.write((uint8_t*)&packet, sizeof(lpkt));
    written += LoRa.write((uint8_t*)&pos, sizeof(lposition));
    int vittu = LoRa.endPacket();
    lastsend = millis();
    Serial.print("lastsend=");
    Serial.println(lastsend);
  }
}







void loop()
{
  Serial.print("Latitude  : ");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Altitude  : ");
  Serial.print(gps.altitude.feet() / 3.2808);
  Serial.println("M");
  Serial.print("Time      : ");
  Serial.print(gps.time.hour());
  Serial.print(":");
  Serial.print(gps.time.minute());
  Serial.print(":");
  Serial.println(gps.time.second());
  Serial.print("Speed     : ");
  Serial.println(gps.speed.kmph()); 
  Serial.println("**********************");

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));


  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  display.drawString(0, 0, "Lat:");
  display.drawString(24, 0, String(gps.location.lat()));
  display.drawString(0, 14, "Lon: ");
  display.drawString(24, 14, String(gps.location.lng()));
  display.drawString(90, 0, "S:");
  display.drawString(100, 0, String(gps.satellites.value()));
  display.drawString(90, 14, "A:");
  display.drawString(100, 14, String(gps.altitude.feet() / 3.2808));
  display.drawString(0, 28, "Time:");
  display.drawString(48, 28, String(gps.time.hour()));
  display.drawString(64, 28, String(gps.time.minute()));
  display.drawString(80, 28, String(gps.time.second()));
  display.drawString(0, 42, "Speed:");
  display.drawString(60, 42, String(gps.speed.kmph())); 
  
  display.display();

  maybeSendPosition();
  Serial.println("kyrpa");

  // send packet
//  LoRa.beginPacket();
//  LoRa.print("hello ");
//  LoRa.print(counter);
//  LoRa.endPacket();
//  Serial.println(". done");

}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (GPS.available())
      gps.encode(GPS.read());
  } while (millis() - start < ms);
}
