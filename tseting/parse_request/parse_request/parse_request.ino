#include <SPI.h>
#include <Ethernet.h>

enum httpStatus{success, notFound};

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xCC
};
IPAddress ip(192, 168, 1, 110);
bool pcRunning = false;
EthernetServer server(80);

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
}

void loop() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        Serial.println("new client");
        String request = getRequest(client);
        Serial.print("request = [");
        Serial.print(request);
        Serial.println("]");
        if (request.equals("")) {
            if (pcRunning) {
                printHttpHeader(success, client);
                client.println("PC is already running");
            } else {
                printHttpHeader(success, client);
                printBoot(client);
                pcRunning = true;
            }
        } else if (request.equals("?boot=on")) {
            printHttpHeader(success, client);
        } else {
            printHttpHeader(notFound, client);
        }
        delay(1);
        client.stop();
        Serial.println("client disconnected");
        Ethernet.maintain();
    }
}

void printHttpHeader(httpStatus http, EthernetClient client) {
    if (http == success) {
        client.println("HTTP/1.1 200 OK");
    } else if (http == notFound) {
        client.println("HTTP/1.1 404 Not Found");
    } else {
        return;
    }
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println();
}

void printBoot(EthernetClient client) {
    /********************************************************
        <!DOCTYPE html>
        <html>
            <form method="get">
                <input type="hidden" name="Boot" value="on">
                <input type="submit" value="Submit">
            </form>
        </html>
    ********************************************************/
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("    <form method=\"get\">");
    client.println("        <input type=\"hidden\" name=\"boot\" value=\"on\">");
    client.println("        <input type=\"submit\" value=\"Submit\">");
    client.println("    </form>");
    client.println("</html>");
}

String getRequest(EthernetClient client) {
    String request = "";
    
    boolean currentLineIsBlank = true;
    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            request += c;
            Serial.write(c);
            // if you've gotten to the end of the line (received a newline
            // character) and the line is blank, the http request has ended,
            // so you can send a reply
            if (c == '\n' && currentLineIsBlank) {
                break;
            }
            if (c == '\n') {
                // you're starting a new line
                currentLineIsBlank = true;
            } else if (c != '\r') {
                // you've gotten a character on the current line
                currentLineIsBlank = false;
            }
        }
    }

    if (request.startsWith("GET")) {
        return request.substring(5, request.indexOf("HTTP/1.1")-1);
    } else {
        Serial.println("ERROR, not a HTTP GET request");
        return "404";
    }
}
