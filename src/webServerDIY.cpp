#include "webServerDIY.h"
#include <WiFi.h>

const char *udpAddress = "192.168.4.255";
const int udpPort = 3333;
const char *ssid = "MPL";
const char *password = "123456789";

WiFiServer server(80);
WiFiUDP udp;
WiFiClient client;


void handleWiFiClient(void)
{
  Serial.println("New Client."); // print a message out the serial port
  String currentLine = "";       // make a String to hold incoming data from the client
  while (client.connected())
  { // loop while the client's connected
    if (client.available())
    {                         // if there's bytes to read from the client,
      char c = client.read(); // read a byte, then
      //Serial.write(c);                  // print it out the serial monitor
      if (c == '\n')
      { // if the byte is a newline character
        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0)
        {
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println(); // Don't delete this line!
          client.println("<!DOCTYPE html><html><head>");
          client.println("<style>");
          // CSS Section to set styles check out W3C School for help
          client.println("body    {background-color: lightblue;font-family: tahoma;}");
          client.println("h1      {color: red;text-align: center;}");
          client.println("p       {font-size: 20px;text-align: center;colour:MediumOrchid}");
          client.println(".button {background-color:blue;color:white;padding:10px 32px;text-align:center;text-decoration:none;display:inline-block;");
          client.println("font-size:16px;margin: 4px 2px;cursor: pointer;width:8%;}");
          client.println("</style>");
          client.println("</head><body>");
          client.println("<h1>MPL - Mobile Party Light</h1>");
          client.println("<p>Select Client Device to control</p>");
          // the content of the HTTP response follows the header:
          client.print("<div style = \"text-align: center;\">");
          // Controls for sending a Turn-ON message
          client.print("<a class=\"button\" href=\"/C01H\">Client 1 Device ON</a>");
          client.print("<a class=\"button\" href=\"/C01L\">Client 1 Device OFF</a>");
          //client.print("<a class=\"button\" href=\"/C01L\">Client 1 Device OFF</a>");
          client.print("<br>"); // New line
          // Controls for sending a Turn-OFF message
          client.print("<a class=\"button\" href=\"/C02H\">Client 2 Device ON</a>");
          client.print("<a class=\"button\" href=\"/C02L\">Client 2 Device OFF</a>");
          //client.print("<a class=\"button\" href=\"/C02L\">Client 2 Device OFF</a>");
          client.print("<br>"); // New line
          // Controls for sending a general or broadcast like message
          client.print("<a class=\"button\" href=\"/CAON\">Turn All Client Devices ON</a>");
          client.print("<a class=\"button\" href=\"/CAOFF\">Turn All Client Devices OFF</a>");
          client.print("</div>");
          // The HTTP response ends with another blank line:
          client.println();
          client.println("</body></html>");
          // break out of the while loop:
          break;
        }
        else
        { // if you got a newline, then clear currentLine:
          currentLine = "";
        }
      }
      else if (c != '\r')
      {                   // if you got anything else but a carriage return character,
        currentLine += c; // add it to the end of the currentLine
      }

      /***** Client-01 High *****/
      if (currentLine.endsWith("GET /C01H"))
      {
        sendData(CLIENT_LIGHTTOWER_1, STATUS_COLOR);
      }

      /***** Client-02 High *****/
      if (currentLine.endsWith("GET /C02H"))
      {
        sendData(CLIENT_LIGHTTOWER_2, STATUS_COLOR);
      }

      /***** Client-01 Low *****/
      if (currentLine.endsWith("GET /C01L"))
      {
        sendData(CLIENT_LIGHTTOWER_1, STATUS_OFF);
      }

      /***** Client-02 Low *****/
      if (currentLine.endsWith("GET /C02L"))
      {
        sendData(CLIENT_LIGHTTOWER_2, STATUS_OFF);
      }

      /***** All Clients High *****/
      if (currentLine.endsWith("GET /CAON"))
      {
        sendData(CLIENT_ALL_LIGHTTOWER, STATUS_MUSIC);
      }

      /***** All Clients Low *****/
      if (currentLine.endsWith("GET /CAOFF"))
      {
        sendData(CLIENT_ALL_LIGHTTOWER, STATUS_OFF);
      }
    }
  }
  // close the connection:
  client.stop();
  Serial.println("Client Disconnected.");
}

void sendData(int value)
{
  udp.beginPacket(udpAddress, udpPort);
  udp.print(value);
  udp.endPacket();
}

void sendData(TargetClient target, Status status)
{
  udp.beginPacket(udpAddress, udpPort);
  udp.print(target);
  udp.print(status);
  udp.endPacket();
}

void sendData(TargetClient target, Status status, int value)
{
  udp.beginPacket(udpAddress, udpPort);
  udp.print(target);
  udp.print(status);
  udp.print(value);
  udp.endPacket();
}

void sendData(TargetClient target, Status status, unsigned int red, unsigned int green, unsigned int blue)
{
  udp.beginPacket(udpAddress, udpPort);
  udp.print(target);
  udp.print(status);
  udp.print(red);
  udp.print(green);
  udp.print(blue);
  udp.endPacket();
}

void initWiFi(void)
{
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  udp.begin(myIP, udpPort);
  server.begin();
  Serial.println("Server started");
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

bool getServerAvailable(void)
{
  client = server.available();
  if (client)
  {
    return true;
  }
  else
  {
    return false;
  }
  
}