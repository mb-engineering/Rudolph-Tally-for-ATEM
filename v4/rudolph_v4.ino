// include files
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>
#include <Rudolph_SD.h>
#include <Rudolph_Error.h>

//variables
int stat_connection = 0;
int PreviewTallyPrevious = 1;
int ProgramTallyPrevious = 1;
int cameraNumber = 0;

// Setup ATEM switcher
ATEMstd AtemSwitcher;


void setup() {

  Serial.begin(115200);
  pinMode(4, OUTPUT);
  delay(2000);
   
  // Read from SD
  readConfigFile("CONFIG.TXT"); //FILE NAME

  String strIP1 = requestSetting("IPATEM");
  IPAddress switcherIp;
  switcherIp.fromString(strIP1);

  String strIP2 = requestSetting("IPCLIENT");
  IPAddress clientIp;
  clientIp.fromString(strIP2);

  String strIP3 = requestSetting("GATEWAY");
  IPAddress gateway;
  gateway.fromString(strIP3);
  
  String camera = requestSetting("CAM");
  cameraNumber = camera.toInt();

  String ssid = requestSetting("SSID");
  String password = requestSetting("PW");

  IPAddress dns(10, 0, 0, 1);
  IPAddress subnet(255, 255, 255, 0);

  //Check for errors
  if (String(ssid) == String(cameraNumber)) {
      feedback_error(7);
      IPAddress clientIp(192, 168, 10, 101);
      IPAddress switcherIp(192, 168, 10, 240);
      IPAddress gateway(192, 168, 10, 1); 
      cameraNumber = 1;
      ssid = "ATEMWIFI";
      password = "password!";
  } else { 
      feedback(1);
  }

  //String to char
  const char *ssid_c = ssid.c_str();
  const char *password_c = password.c_str();

  // Setup and start WIFI
  WiFi.disconnect();
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.config(clientIp,dns,gateway,subnet);
  WiFi.begin(ssid_c, password_c);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }

  //feedback for network settings
  feedback(2);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Waiting for tally of input: ");
  Serial.println(cameraNumber);

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

}


void loop() {

  //One time feedback when connected
  if (stat_connection == 0) {
    if (AtemSwitcher.isConnected() == 1) {
      stat_connection = 1;
      feedback(3);
    }
  }

  //Keeping the connection alive!
  AtemSwitcher.runLoop();
  
  // Check tally status
  int ProgramTally = AtemSwitcher.getProgramTally(cameraNumber);
  int PreviewTally = AtemSwitcher.getPreviewTally(cameraNumber);

  if ((ProgramTallyPrevious != ProgramTally) || (PreviewTallyPrevious != PreviewTally)) { // changed?

    if ((ProgramTally && !PreviewTally) || (ProgramTally && PreviewTally) ) { // only program, or program AND preview
        digitalWrite(4, HIGH);
    } else if (PreviewTally && !ProgramTally) { // only preview
        digitalWrite(4, LOW);
    } else if (!PreviewTally || !ProgramTally) { // neither program or preview
        digitalWrite(4, LOW);
    }

  }

  ProgramTallyPrevious = ProgramTally;
  PreviewTallyPrevious = PreviewTally;    

}
