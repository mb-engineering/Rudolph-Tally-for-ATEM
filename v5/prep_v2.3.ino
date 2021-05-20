
/*********
  Prep for Tally Rudolph Version 2.3
  run once before flashing the main code to your board!
*********/

//include
#include <EEPROM.h>


//functions eeprom
void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');
  EEPROM.commit();
}


String read_String(char add)
{
  int i;
  char data[100];
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

String rx;
String test = "Das ist ein Test";

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }

  writeString(7, "1");
  writeString(9, "12");
  writeString(12, "192.168.178.101");
}

void loop(){
   rx = read_String(7);
   Serial.println("Ab 7: " + rx);
   rx = read_String(9);
   Serial.println("Ab 9: " + rx);
   rx = read_String(12);
   Serial.println("Ab 12: " + rx);
   delay(4000);
}
