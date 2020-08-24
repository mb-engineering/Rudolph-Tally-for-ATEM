// SD card code
File configFile;
uint8_t filePosition;
uint8_t workingIndex = 0;
uint8_t settingNameValuePositions[16][3];
uint8_t temporaryBufferPosition = 0;
char temporaryBuffer[32];

void setPosition(uint8_t mrk) {
  settingNameValuePositions[workingIndex][mrk] = filePosition;
}

uint8_t getPosition(uint8_t idx, uint8_t mrk) {
  return settingNameValuePositions[idx][mrk];
}

void addToBuffer(char inp) {
  temporaryBuffer[temporaryBufferPosition] = inp;
  temporaryBufferPosition++;
}


void resetBuffer() {
  temporaryBufferPosition = 0;
  memset(temporaryBuffer, 0, sizeof(temporaryBuffer));
}

char* requestSetting(char *settingName) {
  for (int i = 0; i < workingIndex; i++) {
    uint8_t nS = getPosition(i,0); //$
    uint8_t eQ = getPosition(i,1); //=
    uint8_t vE = getPosition(i,2); //;
    bool isFound = false;
    resetBuffer();
    for (int g = nS; g < eQ - 1; g++) {
      configFile.seek(g);
      addToBuffer(configFile.read());
    }
    if (strcmp(temporaryBuffer,settingName) == 0) {
      isFound = true;
      resetBuffer();
    }
    for (int g = eQ; g < vE - 1; g++) {
      configFile.seek(g);
      addToBuffer(configFile.read());
    }
    if (isFound) {
      isFound = false;
      return temporaryBuffer;
    }
  }
  return "0";
}

bool readConfigFile(char* fileName) {
  //Serial.println("initializing SD card");
  if (!SD.begin(4)) {
    Serial.println("initializing SD card failed");
    return false;
  }
  else {
    Serial.println("initializing SD card completed");
    Serial.println("opening file: " + String(fileName));
    configFile = SD.open(fileName);
    if (!configFile) {
      Serial.println("error opening file: " + String(fileName));
      return false;
    }
    else {
      Serial.println("file opened");
      Serial.println("searching for marker positions");
      while (configFile.available()) {
        char fileChar = configFile.read();
        filePosition = configFile.position();
        switch (fileChar) {
          case '$':
            setPosition(0);
            break;
          case '=':
            setPosition(1);
            break;
          case ';':
            setPosition(2);
            workingIndex++;
        }
      }
    }
    return true;
  }
}
