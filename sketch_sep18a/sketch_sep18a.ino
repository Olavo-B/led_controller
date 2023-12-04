/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Olavo";
const char* password = "vida1234";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String outputRedState = "off";
String outputGreenState = "off";
String outputBlueState = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(LED_BUILTIN, OUTPUT);

  // Set outputs to LOW
  digitalWrite(LED_BUILTIN, LOW);


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(LED_BUILTIN, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(LED_BUILTIN, LOW);
            }

            /* Turns RBG on and off*/
            else if (header.indexOf("GET /red/on") >= 0) {
              Serial.println("Red on");
              outputRedState = "on";
              neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0); // Red

            }else if (header.indexOf("GET /red/off") >= 0) {
              Serial.println("Red off");
              outputRedState = "off";
              digitalWrite(LED_BUILTIN, LOW);    // Turn the RGB LED off
            }else if (header.indexOf("GET /green/on") >= 0) {
              Serial.println("Green on");
              outputGreenState = "on";
              neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green
            }else if (header.indexOf("GET /green/off") >= 0) {
              Serial.println("Green off");
              outputGreenState = "off";
              digitalWrite(LED_BUILTIN, LOW);    // Turn the RGB LED off
            }else if (header.indexOf("GET /blue/on") >= 0) {
              Serial.println("Blue on");
              outputBlueState = "on";
              neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); // Blue
            }else if (header.indexOf("GET /blue/off") >= 0) {
              Serial.println("Blue off");
              outputBlueState = "off";
              digitalWrite(LED_BUILTIN, LOW);    // Turn the RGB LED off
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");

            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            /* Create buttons for RGB led controller*/
            client.println("<p>GPIO RED - State " + outputRedState + "</p>");

            if (outputRedState=="off") {
              client.println("<p><a href=\"/red/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/red/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("<p>GPIO GREEN - State " + outputGreenState + "</p>");

            if (outputGreenState=="off") {
              client.println("<p><a href=\"/green/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/green/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("<p>GPIO BLUE - State " + outputBlueState + "</p>");

            if (outputBlueState=="off") {
              client.println("<p><a href=\"/blue/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/blue/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
