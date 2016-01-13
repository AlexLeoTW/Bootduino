/*
 * Occupied Pin:
 * 10, 11, 12: Arduinoo Shild R3 SPI bus
 * 10:  select W5100
 * 4:   select SD card
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xCC};
IPAddress ip(192, 168, 1, 120);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress dns1(192, 168, 1, 1);
EthernetServer server(80);
File htmlFile;

void setup() {
    Serial.begin(9600);

    Serial.println("Initinalizing Ethernet...");
    Ethernet.begin(mac, ip, dns1, gateway, subnet);

    Serial.println("Initinalizing SD card...");
    if (SD.begin(4)) {
        Serial.println("ERROR while initializing SD card.");
        return;
    }
    
    Serial.println("Initializing HTTP Server...");
    if (!setupHttpServer()){
        Serial.println("ERROR while initializing HTTP server.");
    }
}

void loop() {
    EthernetClient client = server.available();
    /*if (client) { // Client has data available for reading
        while (client.connect()) { // Client is connected or has unread data
            if (client.available()) { // the number of bytes available
                char c = client.read();
            }
            delay(1);
            client.stop();
            Serial.println("Client Disconnected.");
        }
    }*/
    while (client.connected()) {
        if (client.available()) {
            char thisChar = client.read();
            
        } else /* client waiting */ {
            delay(1);
        }
    }
}

bool setupHttpServer() {
    if (!SD.exists("/web/index.html")) {
        Serial.println("ERROR, /web/index.html file not found.");
        return false;
    }
    server.begin();
    Serial.print("server started at: ");
    Serial.print(Ethernet.localIP());
    Serial.println(":80");
}

bool bufferHttpRequest(char c) {
    
}
