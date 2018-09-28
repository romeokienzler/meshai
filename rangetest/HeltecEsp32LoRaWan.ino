#include <dummy.h>  //silence warnings from Arduino IDE
//combination of OTAA code and GPS for Heltec ESP32 OLED LoRa module
// NCB Fri 12th Jan 2018
// Link with LMiC library available here: https://github.com/matthijskooijman/arduino-lmic/
// GPS get_coords function adapted from here: https://github.com/brady-aiello/Seeeduino_LoRaWAN_for_hybrid_gateways
#include <HardwareSerial.h>
#include <TinyGPS++.h>

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <U8x8lib.h>

#define BUILTIN_LED 25
#define GPS_RX 22
#define GPS_TX 23


// The TinyGPS++ object
TinyGPSPlus gps;

typedef struct __attribute__ ((packed)) {
  float latitude;
  float longitude;
  uint32_t altitude;
  uint32_t hdop;
  uint32_t satellites;
} GpsData;

GpsData gpsData;

typedef union {
  GpsData d;               
  unsigned char bytes[20];   
} floatArr2Val;
floatArr2Val latlong;


char s[16]; // used to sprintf for OLED display
char recBuff[40];

HardwareSerial GPSSerial(1);


// the OLED used - pins for Clock, Data and Reset
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16);

// Enter App EUI in little-endian format (reverse as shown in TTN console)
static const u1_t PROGMEM APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}
// Enter Device EUI in little-endian format (reverse as shown in TTN console)
static const u1_t PROGMEM DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}
// Enter the Application key from TTN
static const u1_t PROGMEM APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

static osjob_t sendjob;
// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 30;
const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = {26, 33, 32},
};

void get_coords () {
  while (GPSSerial.available())
  {
    int i = GPSSerial.read();
//    Serial.print(char(i));
    gps.encode(i);
  }
  Serial.println();
  gpsData.latitude  = gps.location.lat();
  gpsData.longitude = gps.location.lng();
  double alt = (gps.altitude.meters()+500) * 100;
  gpsData.altitude = (uint32_t)round(alt);
  gpsData.hdop = round(gps.hdop.hdop() * 100);
  gpsData.satellites = gps.satellites.value();
  Serial.print("Lat : ");
  Serial.println(gpsData.latitude);
  Serial.print("Lon : ");
  Serial.println(gpsData.longitude);
  Serial.print("Alt : ");
  Serial.println((double)gpsData.altitude/100.0 - 500.0);
  Serial.print("Hdop : ");
  Serial.println((double)gpsData.hdop/100.0);
  Serial.print("Satellite count : ");
  Serial.println(gpsData.satellites);
  // Only update if location is valid and has changed
  if ((gpsData.latitude && gpsData.longitude) && (gpsData.latitude != latlong.d.latitude
      || gpsData.longitude != latlong.d.longitude || gpsData.altitude != latlong.d.altitude)) {
    memcpy(&latlong.d, &gpsData, sizeof(GpsData));    
    for (int i = 0; i < sizeof(GpsData); i++) {
      if (latlong.bytes[i] < 10) {
        Serial.print("0");
      }
      Serial.print(latlong.bytes[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  u8x8.setCursor(0, 2);
  u8x8.print("Lat: ");
  u8x8.setCursor(5, 2);
  sprintf(s, "%f", gpsData.latitude);
  u8x8.print(s);
  u8x8.setCursor(0, 3);
  u8x8.print("Lng: ");
  u8x8.setCursor(5, 3);
  sprintf(s, "%f", gpsData.longitude);
  u8x8.print(s);
  u8x8.setCursor(0, 4);
  u8x8.print("Alt: ");
  u8x8.setCursor(5, 4);
  sprintf(s, "%.2fm", (double)gpsData.altitude/100.0 - 500);
  u8x8.print(s);
  u8x8.setCursor(0, 5);
  u8x8.print("Sat: ");
  u8x8.setCursor(5, 5);
  sprintf(s, "%i", gpsData.satellites);
  u8x8.print(s);
}

void onEvent (ev_t ev) {
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      u8x8.drawString(0, 7, "EV_SCAN_TIMEOUT");
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      u8x8.drawString(0, 7, "EV_BEACON_FOUND");
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      u8x8.drawString(0, 7, "EV_BEACON_MISSED");
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      u8x8.drawString(0, 7, "EV_BEACON_TRACKED");
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      u8x8.drawString(0, 7, "EV_JOINING   ");
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      u8x8.drawString(0, 7, "EV_JOINED    ");
      // Disable link check validation (automatically enabled
      // during join, but not supported by TTN at this time).
      LMIC_setLinkCheckMode(0);
      LMIC_enableTracking (3);
      LMIC_setPingable (3);
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      u8x8.drawString(0, 7, "EV_RFUI");
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      u8x8.drawString(0, 7, "EV_JOIN_FAILED");
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      u8x8.drawString(0, 7, "EV_REJOIN_FAILED");
      //break;
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      u8x8.drawString(0, 7, "EV_TXCOMPLETE");
      digitalWrite(BUILTIN_LED, LOW);
      if (LMIC.txrxFlags & TXRX_ACK) {
        Serial.println(F("Received ack"));
        u8x8.drawString(0, 7, "Received ACK");
      }
      if (LMIC.dataLen) {
        memcpy(recBuff, LMIC.frame+LMIC.dataBeg, (LMIC.dataLen < 40) ? LMIC.dataLen : 39);
        recBuff[LMIC.dataLen]= 0;
        Serial.print(F("Received "));
        u8x8.drawString(0, 6, "RX ");
        Serial.print(LMIC.dataLen);
        u8x8.setCursor(4, 6);
        u8x8.printf("%i bytes", LMIC.dataLen);
        Serial.print(F(" bytes of payload : "));
        Serial.println(recBuff);
        u8x8.setCursor(0, 7);
        u8x8.printf("RSSI %d SNR %.1d", LMIC.rssi, LMIC.snr);
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      u8x8.drawString(0, 7, "EV_LOST_TSYNC");
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      u8x8.drawString(0, 7, "EV_RESET");
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      u8x8.drawString(0, 7, "EV_RXCOMPLETE");
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      u8x8.drawString(0, 7, "EV_LINK_DEAD");
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      u8x8.drawString(0, 7, "EV_LINK_ALIVE");
      break;
    default:
      Serial.println(F("Unknown event"));
      u8x8.setCursor(0, 7);
      u8x8.printf("UNKNOWN EVENT %d", ev);
      break;
  }
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
    u8x8.drawString(0, 7, "OP_TXRXPEND, not sent");
  } else {
    // Prepare upstream data transmission at the next possible time.
    get_coords();
    //LMIC_setTxData2(1, (uint8_t*) coords, sizeof(coords), 0);
    LMIC_setTxData2(1, latlong.bytes, 20, 0);
    Serial.println(F("Packet queued"));
    u8x8.drawString(0, 7, "PACKET QUEUED");
    digitalWrite(BUILTIN_LED, HIGH);
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("LoRa device");
  
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  GPSSerial.setTimeout(2);
  
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 1, "LoRa Dev");
 
//  SPI.begin(5, 19, 27);
  // LMIC init
  Serial.println("LoRa device LMIC init");
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC.dn2Dr = DR_SF9;
  // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  Serial.println("LoRa device end setup");
}


void loop() {
  os_runloop_once();
}
