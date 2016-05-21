#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>

/*
 * ===== below as eps8266_151230_udpEchoWithAccessPoint =====
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

ESP8266WebServer myServer(80);

// UDP realated
static WiFiUDP myWifiUDP;
static char receivedBuffer[255];
static const int kLocalPort = 7000;

void handleRoot() {
    myServer.send(200, "text/html", "<h1>You are connected</h1>");
}

static void WiFi_setup()
{
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

    myServer.on("/", handleRoot);
    myServer.begin();
    Serial.println("HTTP server started");
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
    
    myWifiUDP.beginPacket( myWifiUDP.remoteIP(), myWifiUDP.remotePort() );
    myWifiUDP.write("received:");
    myWifiUDP.write(receivedBuffer);
    myWifiUDP.endPacket();
}

void loop() {
    myServer.handleClient();
    processUdpReceive();
}
