void setup() {
  pinMode(A4, INPUT);
  Serial.begin(9600); 
}

void loop() {
  int buttonState = analogRead(A4);
  Serial.println(buttonState);
}
