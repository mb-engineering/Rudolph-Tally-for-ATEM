//Rudolph Error Codes


void feedback_error(int number) {
  for(int i=1;i<=number;i++){
    digitalWrite(4, HIGH);
    delay(200);
    digitalWrite(4, LOW);
    delay(200);
  }
delay(2000);
}

void feedback(int number) {
  for(int i=1;i<=number;i++){
    digitalWrite(4, HIGH);
    delay(500);
    digitalWrite(4, LOW);
    delay(1000);
  }
delay(2000);
}