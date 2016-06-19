#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <SPI.h>
#include <SD.h>

/*
 * v0.3 2016 Jun. 19
 *   - add time command
 *     + add procCommand()
 *     + add [kPort_command]
 *   - refactor to accomodate time command
 *     + rename processUdpReceive() to processUdpReceiveData()
 *     + rename [myWifiUDP] to [myWifiUDP_data]
 *     + rename [kLocalPort] to [kPort_data]
 * v0.2 2016 Jun. 2
 *   - fix bug > SD write with extra CRLF
 * v0.1 2016 May 21
 *   - add SD_write()
 *   - add SD_setup()
 *   - add softAPConfig() to set IP address
 *   - processUdpReceive() does not have UDP echo back
 * ===== below as [eps8266_151230_udpEchoWithAccessPoint] =====
 * v0.2 2015 Dec. 30
 *   - add processUdpReceive()
 *   - WiFi_setup() has myWifiUDP_data.begin()
 *   - add kPort_data
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
// 1. data
static WiFiUDP myWifiUDP_data;
static char receivedBuffer[255];
static const int kPort_data = 7000;
// 2. command
static WiFiUDP myWifiUDP_command;
static const int kPort_command = 7001;

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
    myWifiUDP_data.begin(kPort_data);
    myWifiUDP_command.begin(kPort_command);
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
        s_myFile.print(rcvdPtr);
        s_myFile.close();
        Serial.println("done.");
    } else {
        Serial.println("error opening test.txt");
    }
}


void processUdpReceiveData()
{
    int rcvdSize = myWifiUDP_data.parsePacket();
    if (rcvdSize == 0) {
//        delay(100);
        return;
    }
    
    Serial.println("received data");
    
    int len = myWifiUDP_data.read(receivedBuffer, 255);
    if (len == 0) {
        return;
    }
    receivedBuffer[len] = 0x00;
    SD_write(receivedBuffer);
}

void procCommand(char *cmdPtr)
{
    if (strncmp(cmdPtr, "time", 4) == 0) {
        Serial.println("received time command");
    }
}

void processUdpReceiveCommand()
{
    int rcvdSize = myWifiUDP_command.parsePacket();
    if (rcvdSize == 0) {
//        delay(100);
        return;
    }

    Serial.println("received command");
    
    int len = myWifiUDP_command.read(receivedBuffer, 255);
    if (len == 0) {
        return;
    }

    receivedBuffer[len] = 0x00;

    Serial.println(receivedBuffer);

    procCommand(receivedBuffer);
}

void loop() {
    processUdpReceiveData();
    processUdpReceiveCommand();

    delay(100); // msec
}
