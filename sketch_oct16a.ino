
// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials for both networks
const char* ssid1 = "Rangana Note 20";
const char* password1 = "12345678";

const char* ssid2 = "UOC_Staff";
const char* password2 = "admin106";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputState[16] = { "off", "off", "off", "off", "off", "off", "off", "off", "off", "off", "off", "off", "off", "off", "off", "off" };

// Assign output variables to GPIO pins
const int outputPins[16] = { 2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 21, 22, 25, 32, 26, 27 };

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void connectWiFi() {
  Serial.println("Trying to connect to the first network...");
  WiFi.begin(ssid1, password1);
  
  // Try to connect to the first network for 10 seconds
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to the first network!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to the first network. Trying the second network...");
    WiFi.begin(ssid2, password2);
    
    // Try to connect to the second network for another 10 seconds
    startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      Serial.print(".");
      delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to the second network!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nFailed to connect to both networks.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Set all pins to high impedance mode (input)
  for (int i = 0; i < 16; i++) {
    pinMode(outputPins[i], INPUT);
  }

  // Call the Wi-Fi connection function
  connectWiFi();
  
  // Start web server if connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    server.begin();
  }
}

void handleReading(int reading) {
  // Set all pins to high impedance mode
  for (int i = 0; i < 16; i++) {
    pinMode(outputPins[i], INPUT);
    outputState[i] = "off";
  }

  // Set specific pins to LOW (as output) based on the reading
  switch (reading) {
    case 1:
      pinMode(2, OUTPUT); digitalWrite(2, LOW); outputState[0] = "on";
      pinMode(14, OUTPUT); digitalWrite(14, LOW); outputState[5] = "on";
      pinMode(21, OUTPUT); digitalWrite(21, LOW); outputState[10] = "on";
      pinMode(27, OUTPUT); digitalWrite(27, LOW); outputState[15] = "on";
      break;

    case 2:
      pinMode(5, OUTPUT); digitalWrite(5, LOW); outputState[2] = "on";
      pinMode(16, OUTPUT); digitalWrite(16, LOW); outputState[7] = "on";
      pinMode(17, OUTPUT); digitalWrite(17, LOW); outputState[8] = "on";
      pinMode(32, OUTPUT); digitalWrite(32, LOW); outputState[13] = "on";
      break;

    case 3:
      pinMode(2, OUTPUT); digitalWrite(2, LOW); outputState[0] = "on";
      pinMode(15, OUTPUT); digitalWrite(15, LOW); outputState[6] = "on";
      pinMode(18, OUTPUT); digitalWrite(18, LOW); outputState[9] = "on";
      pinMode(27, OUTPUT); digitalWrite(27, LOW); outputState[15] = "on";
      break;

    case 4:
      pinMode(4, OUTPUT); digitalWrite(4, LOW); outputState[1] = "on";
      pinMode(16, OUTPUT); digitalWrite(16, LOW); outputState[7] = "on";
      pinMode(17, OUTPUT); digitalWrite(17, LOW); outputState[8] = "on";
      pinMode(26, OUTPUT); digitalWrite(26, LOW); outputState[14] = "on";
      break;
  }
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {  // If a new client connects
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {  // if there's bytes to read from the client,
        char c = client.read();  // read a byte, then
        Serial.write(c);         // print it out the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          if (currentLine.length() == 0) {  // if the current line is blank, you've reached the end of the request
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Handle "Reading" buttons
            if (header.indexOf("GET /reading1") >= 0) {
              Serial.println("Reading 1 pressed");
              handleReading(1);
            } else if (header.indexOf("GET /reading2") >= 0) {
              Serial.println("Reading 2 pressed");
              handleReading(2);
            } else if (header.indexOf("GET /reading3") >= 0) {
              Serial.println("Reading 3 pressed");
              handleReading(3);
            } else if (header.indexOf("GET /reading4") >= 0) {
              Serial.println("Reading 4 pressed");
              handleReading(4);
            }

            // Generate HTML page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}");
            client.println("</style></head>");

            client.println("<body><h1>ESP32 GPIO Control</h1>");
            client.println("<p>");
            client.println("<a href=\"/reading1\"><button class=\"button\">Reading 1</button></a>");
            client.println("<a href=\"/reading2\"><button class=\"button\">Reading 2</button></a>");
            client.println("<a href=\"/reading3\"><button class=\"button\">Reading 3</button></a>");
            client.println("<a href=\"/reading4\"><button class=\"button\">Reading 4</button></a>");
            client.println("</p>");

            for (int i = 0; i < 16; i++) {
              client.println("<p>GPIO " + String(outputPins[i]) + " - State " + outputState[i] + "</p>");
              if (outputState[i] == "off") {
                client.println("<p><a href=\"/" + String(outputPins[i]) + "/on\"><button class=\"button\">ON</button></a></p>");
              } else {
                client.println("<p><a href=\"/" + String(outputPins[i]) + "/off\"><button class=\"button button2\">OFF</button></a></p>");
              }
            }

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
