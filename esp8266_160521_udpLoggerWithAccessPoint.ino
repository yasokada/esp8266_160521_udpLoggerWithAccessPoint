#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <SPI.h>
#include <SD.h>

/*
 * v0.1 2016 May 21
 *   - add SD_write()
 *   - add SD_setup()
 *   - add softAPConfig() to set IP address
 *   - processUdpReceive() does not have UDP echo back
 * ===== below as [eps8266_151230_udpEchoWithAccessPoint] =====
 * v0.2 2015 Dec. 30
 *   - add processUdpReceive()
 *   - WiFi_setup() has myWifiUDP.begin()
 *   - add kLocalPort
 *   - add receivedBuffer[]
 *   - include WiFiUDP.h
 * v0.1 2015 Dec. 30
 *   - add Serial_setup()
 *   - add WiFi_setup()
 *   - add handleRoot()
 *   - add loop()
 *   - add setup()
 */

// Access point related
static const char *kSsid = "esp8266";
static const char *kPassword = "12345678";

// UDP realated
static WiFiUDP myWifiUDP;
static char receivedBuffer[255];
static const int kLocalPort = 7000;

// SD related
#define SD_CS (4)
static File s_myFile;

bool SD_setup()
{
    Serial.print("Initializing SD card...");

    if (!SD.begin(SD_CS)) {
      Serial.println("initialization failed!");
      return false;
    }
    Serial.println("initialization done.");
    return true;
}

static void WiFi_setup()
{
    WiFi.softAPConfig(IPAddress(192, 168, 79, 2), IPAddress(192, 168, 79, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(kSsid, kPassword);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    myWifiUDP.begin(kLocalPort);
}

static void Serial_setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.print("Configuring access point...");  
}

void setup() {
    pinMode(4, OUTPUT);
    delay(1000);

    Serial_setup();
    WiFi_setup();
    SD_setup();
}

void SD_write(char *rcvdPtr)
{
    s_myFile = SD.open("test.txt", FILE_WRITE);

    if (s_myFile) {
        Serial.print("Writing to test.txt...");
        s_myFile.println(rcvdPtr);
        s_myFile.close();
        Serial.println("done.");
    } else {
        Serial.println("error opening test.txt");
    }
}


void processUdpReceive()
{
    int rcvdSize = myWifiUDP.parsePacket();
    if (rcvdSize == 0) {
        delay(100);
        return;
    }
    
    Serial.println("received");
    
    int len = myWifiUDP.read(receivedBuffer, 255);
    if (len == 0) {
        return;
    }
    receivedBuffer[len] = 0x00;
    SD_write(receivedBuffer);
}

void loop() {
    processUdpReceive();
}
