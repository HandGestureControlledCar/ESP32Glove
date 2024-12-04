const int potPin1 = 34;
const int potPin2 = 35;

int potValue1 = 0, potValue2 = 0;
// float vout;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);
}

void loop() {
  // Reading potentiometer value
  potValue1 = analogRead(potPin1);
  potValue2 = analogRead(potPin2);
  //vout = (potValue * 5.0) / 1.0;
  Serial.print("Flex 1: ");
  Serial.println(potValue1);
  Serial.print("Flex 2: ");
  Serial.println(potValue2);
  delay(500);
}