
/*********
  Tally Rudolph Version 2.3

  Based on the work of Kasper Skårhøj
  https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering
  
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  
  WiFi Manager
  https://github.com/tzapu/WiFiManager
  
*********/

// includes
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>
#include "rw-eeprom.h"


// Setup variables
int PreviewTallyPrevious = 1;
int ProgramTallyPrevious = 1;
int cameraNumber = 0;
int reboot = 0;
String rx;
IPAddress switcherIp(192, 168, 10, 240);

// Prepare Webserver
WiFiServer server(80);  // Set web server port number to 80
String header;  // Variable to store the HTTP request

// Setup ATEM switcher
ATEMstd AtemSwitcher;

void setup() {

  // Start services basic
  Serial.begin(115200);
  EEPROM.begin(512);
  WiFiManager wifiManager; // Local intialization. Once its business is done, there is no need to keep it around
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

  // Read from eeprom
  rx = read_String(9); // Read from eeprom position 10
  cameraNumber = rx.toInt();  // Convert to int and save to cameraNumber
  rx = read_String(12);  // Read from eeprom position 12
  Serial.println("AUSGELESEN IP: " + rx);
  switcherIp.fromString(rx);  // Convert to IP and save to switcherIp
  
  Serial.println();
  Serial.println("Mischer IP:");
  Serial.println(switcherIp);
  Serial.println("Camera:");
  Serial.println(cameraNumber);
  
  rx = read_String(7);  // Read from eeprom position 12
  if (rx == "1") {
    writeString(7, "0");
    wifiManager.resetSettings();  // reset Wifi setting
    }
  
  // set custom ip and name for portal
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,0,1), IPAddress(10,0,0,1), IPAddress(255,255,255,0));
  wifiManager.autoConnect("Rudolph_Tally");

  // Start services for loop
  server.begin();
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();
  
}

void loop(){
  WiFiManager wifiManager;
  WiFi.mode(WIFI_STA);  // Switch to station mode
  AtemSwitcher.runLoop();   // Keeping the connection alive!


  

  // TALLY START   TALLY START   TALLY START   TALLY START
  
  int ProgramTally = AtemSwitcher.getProgramTally(cameraNumber);
  int PreviewTally = AtemSwitcher.getPreviewTally(cameraNumber);

  if ((ProgramTallyPrevious != ProgramTally) || (PreviewTallyPrevious != PreviewTally)) { // changed?

    if ((ProgramTally && !PreviewTally) || (ProgramTally && PreviewTally) ) { // only program, or program AND preview
        digitalWrite(12, HIGH);
    } else if (PreviewTally && !ProgramTally) { // only preview
        digitalWrite(12, LOW);
    } else if (!PreviewTally || !ProgramTally) { // neither program or preview
        digitalWrite(12, LOW);
    }

  }

  ProgramTallyPrevious = ProgramTally;
  PreviewTallyPrevious = PreviewTally;
  
  // TALLY END   TALLY END   TALLY END   TALLY END


  

  String myIP = WiFi.localIP().toString();  // IP of ESP converted to string
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          String input = header; 
          String saveCam = read_String(9);
          String saveIP = read_String(12);
          
          if (currentLine.length() == 0) {
            
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<style>");
            client.println(".c,body{text-align:center;font-family:verdana}div,input{padding:5px;font-size:1em;margin:5px 0;box-sizing:border-box;}input,button,.msg{border-radius:.3rem;width: 100%},input[type=radio]{width: auto}button,input[type='button'],input[type='submit']{cursor:pointer;border:0;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%}input[type='file']{border:1px solid #1fa3ec}.wrap {text-align:left;display:inline-block;min-width:260px;max-width:500px}a{color:#000;font-weight:700;text-decoration:none}a:hover{color:#1fa3ec;text-decoration:underline}.q{height:16px;margin:0;padding:0 5px;text-align:right;min-width:38px;float:right}.q.q-0:after{background-position-x:0}.q.q-1:after{background-position-x:-16px}.q.q-2:after{background-position-x:-32px}.q.q-3:after{background-position-x:-48px}.q.q-4:after{background-position-x:-64px}.q.l:before{background-position-x:-80px;padding-right:5px}.ql .q{float:left}.q:after,.q:before{content:'';width:16px;height:16px;display:inline-block;background-repeat:no-repeat;background-position: 16px 0;background-image:url('');}@media (-webkit-min-device-pixel-ratio: 2),(min-resolution: 192dpi){.q:before,.q:after {background-image:url('');background-size: 95px 16px;}}.msg{padding:20px;margin:20px 0;border:1px solid #eee;border-left-width:5px;border-left-color:#777}.msg h4{margin-top:0;margin-bottom:5px}.msg.P{border-left-color:#1fa3ec}.msg.P h4{color:#1fa3ec}.msg.D{border-left-color:#dc3630}.msg.D h4{color:#dc3630}.msg.S{border-left-color: #5cb85c}.msg.S h4{color: #5cb85c}dt{font-weight:bold}dd{margin:0;padding:0 0 0.5em 0;min-height:12px}td{vertical-align: top;}.h{display:none}button.D{background-color:#dc3630}body.invert,body.invert a,body.invert h1 {background-color:#060606;color:#fff;}body.invert .msg{color:#fff;background-color:#282828;border-top:1px solid #555;border-right:1px solid #555;border-bottom:1px solid #555;}body.invert .q[role=img]{-webkit-filter:invert(1);filter:invert(1);}input:disabled {opacity: 0.5;}");
            client.println("</style>");
            client.println("<noscript>");
            client.println("Please activate JavaScript.");
            client.println("</noscript>");
            client.println("<script>");
            client.println("function plausicheck() {");
            client.println("if(address_test()) {");
            client.println("if(!camnr_test())");
            client.println("alert('Please enter a camera number between 1 and 99.');");
            client.println("else");
            client.println("document.getElementById('config_form').submit();");
            client.println("}");
            client.println("else");
            client.println("alert('Please enter an IPv4 address in this format: xxx.xxx.xxx.xxx, e.g. 192.168.10.240');");
            client.println("}");
            client.println("function address_test() {");
            client.println("let ipv4 = document.getElementById('ip').value;");
            client.println("if (ipv4 == '255.255.255.255' || ipv4 == '127.0.0.0' || ipv4 == '127.0.0.1' || ipv4 == '0.0.0.0' )");
            client.println("return false;");
            client.println("let regex = new RegExp(/^(([0-9]{1,3}.){3}[0-9]{1,3})$/);");
            client.println("if(regex.test(ipv4)){");
            client.println("let arInputCount = ipv4.split('.').length - 1;");
            client.println("if (arInputCount != '3')");
            client.println("return false;");
            client.println("let arInput = ipv4.split('.')");
            client.println("for(let i of arInput){");
            client.println("if(i.length > 1 && i.charAt(0) === '0')");
            client.println("return false;");
            client.println("else {");
            client.println("if(parseInt(i) < 0 || parseInt(i) >=256)");
            client.println("return false;");
            client.println("}");
            client.println("}");
            client.println("}");
            client.println("else");
            client.println("return false;");
            client.println("return true;");
            client.println("}");
            client.println("function camnr_test() {");
            client.println("let camnr = document.getElementById('camera').value;");
            client.println("let regex = new RegExp(/^[0-9]+$/);");
            client.println("if(regex.test(camnr)){");
            client.println("if(parseInt(camnr) < 1 || parseInt(camnr) > 99)");
            client.println("return false;");
            client.println("else");
            client.println("return true;");
            client.println("}");
            client.println("return false;");
            client.println("}");
            client.println("function wifireset() {");
            client.println("if (confirm('Are you sure you want to reset your Wifi-configuration?')) {");
            client.println("document.getElementById('reset_form').submit();");
            client.println("}");
            client.println("}");
            client.println("</script>");
            client.println("</head>");
            client.println("<body>");
            client.println("<div class='wrap'>");

                if (header.indexOf("GET /config") >= 0) {       //update form

                  client.println("<form action='/upd' id='config_form' method='GET'>");
                  client.println("<label for='ipadress'>IP Address Mixer:</label><br>");
                  client.println("<input type='text' id='ip' name='ip' value='" + saveIP + "'><br><br>");
                  client.println("<label for='camera'>Camera Number:</label><br>");
                  client.println("<input type='text' id='camera' name='camera' value='" + saveCam + "'><br>");
                  client.println("<input type='hidden' id='end' name='end' value='end'><br>");
                  client.println("<input type='button' onclick ='plausicheck()' value='Save configuration'><br>");
                  client.println("</form><br>");
                  client.println("<form action='/ident' id='ident_form'>");
                  client.println("<input type='submit' value='Identify device'>");
                  client.println("</form><br>");
                  client.println("<form action='/wifireset' id='reset_form'>");
                  client.println("<input type='button' onclick ='wifireset()' value='Reset your Wifi'>");
                  client.println("</form><br>");
                  

                 } else if (header.indexOf("GET /wifireset") >= 0) {         // wifi reset

                 client.println("Wifi configuration resetted!<br><br>");
                 reboot = 1;
                 writeString(7, "1");
                 

                 } else if (header.indexOf("GET /ident") >= 0) {         // identify part
                  
                  digitalWrite(12, HIGH);
                  client.println("Tally is turned on. <br><br>");
                  client.println("<form action='/tallyoff' id='tallyoff_form'>");
                  client.println("<input type='submit' value='Turn tally off'>");
                  client.println("</form><br>");
                  client.println("<form action='/config' id='config_form'>");
                  client.println("<input type='submit' value='Rudolph Tally configuration'>");
                  client.println("</form><br>");
                  client.println("<form action='/wifireset' id='reset_form'>");
                  client.println("<input type='button' onclick ='wifireset()' value='Reset your Wifi'>");
                  client.println("</form><br>");


                 } else if (header.indexOf("GET /tallyoff") >= 0) {         // identify part
                  
                  digitalWrite(12, LOW);
                  client.println("Tally is turned off.<br><br>");
                  client.println("<form action='/ident' id='ident_form'>");
                  client.println("<input type='submit' value='Identify device'>");
                  client.println("</form><br>");
                  client.println("<form action='/config' id='config_form'>");
                  client.println("<input type='submit' value='Rudolph Tally configuration'>");
                  client.println("</form><br>");
                  client.println("<form action='/wifireset' id='reset_form'>");
                  client.println("<input type='button' onclick ='wifireset()' value='Reset your Wifi'>");
                  client.println("</form><br>");
                    
                 } else if (header.indexOf("GET /upd") >= 0) {         // update part

                   if (header.indexOf("GET /upd?ip=") >= 0 && header.indexOf("&camera=") >= 0) {
                    int posip = input.indexOf("ip=");
                    int poskamera = input.indexOf("&camera=");
                    int posend = input.indexOf("&end");
                                      
                    String mixerip = input.substring(posip+3, poskamera);
                    String camera = input.substring(poskamera+8, posend);
                    writeString(12, mixerip);
                    writeString(9, camera);
                    switcherIp.fromString(mixerip);
                    cameraNumber = camera.toInt();
                    
                    client.println("IP Address Mixer: " + mixerip + "<br>");                 
                    client.println("Camera Number: " + camera + "<br><br>");
                    Serial.println("IP Address Mixer: = " + mixerip);
                    Serial.println("Camera Number: = " + camera);
                    client.println("Please restart your Rudolph Tally!");
                    reboot = 1;
                   }
 
                 } else {          //basic response

                  int led = digitalRead(12);
                  client.println("<h1>Setup for your Rudolph</h3><br><br>");
                  client.println("<h3>Current configuration:</h3><br>");
                  client.println("IP Address Mixer: " + saveIP + "<br>");                 
                  client.println("Camera Number: " + saveCam + "<br><br>");
                  client.println("<form action='/config' id='config_form'>");
                  client.println("<input type='submit' value='Change configuration'>");
                  client.println("</form><br>");
                    if (led == HIGH) {                    
                     client.println("<form action='/tallyoff' id='tallyoff_form'>");
                     client.println("<input type='submit' value='Turn tally off'>");
                     client.println("</form><br>");
                    } else {
                     client.println("<form action='/ident' id='ident_form'>");
                     client.println("<input type='submit' value='Identify device'>");
                     client.println("</form><br>");
                    }
                  client.println("<form action='/wifireset' id='reset_form'>");
                  client.println("<input type='button' onclick ='wifireset()' value='Reset your Wifi'>");
                  client.println("</form><br>");
                    
                 }

            client.println("<br><br><div class='msg'>The IP of your Tally is " + myIP + "</div>");      
            client.println("</div>");
            client.println("</body>");
            client.println("</html>");      
            client.println(); // The HTTP response ends with another blank line
            break;   // Break out of the while loop
              
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
  
    header = "";  // Clear the header variable
    client.stop();  // Close the connection

    if (reboot == 1) {
      delay(2000);  //wait to display html site
      ESP.restart();
      }
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
