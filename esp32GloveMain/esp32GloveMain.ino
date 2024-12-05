#include <Wire.h>
#include <WiFi.h>

#define ADXL345_ADDRESS 0x1D
#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31
#define DATAX0 0x32
#define I2C_SDA 21
#define I2C_SCL 22

const char* ssid = "car";
const char* password = "123456789";
const char* serverIP = "192.168.4.1";
const int serverPort = 80;

float thresholdX = 0.4;
float thresholdY = 0.7;

WiFiClient client;
bool connected = false;

const int flexiPort1 = 34;
const int flexiPort2 = 35;

int flexiValue1 = 0, flexiValue2 = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(flexiPort1, INPUT);
  pinMode(flexiPort2, INPUT);

  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize the ADXL345 sensor
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(POWER_CTL);
  Wire.write(0x08); // Wake up the sensor
  Wire.endTransmission();

  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(DATA_FORMAT);
  Wire.write(0x0B); // Full resolution, ±16g
  Wire.endTransmission();

  // Setup WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  String direction = "S";
  String speed = "0";
  String horn = "x";

  // Reading flexi values
  flexiValue1 = analogRead(flexiPort1);
  flexiValue2 = analogRead(flexiPort2);

  // Serial.print("Flex 1: ");
  // Serial.println(flexiValue1);
  // Serial.print("Flex 2: ");
  // Serial.println(flexiValue2);

  if(flexiValue1 == 0) speed = "0";
  else if(flexiValue1 < 50) speed = "1";
  else if(flexiValue1 < 100) speed = "2";
  else if(flexiValue1 < 300) speed = "3";
  else if(flexiValue1 < 500) speed = "4";
  else speed = "5";

  if(flexiValue2 == 0) horn = "x";
  else horn = "H";


  // Ensure TCP connection
  if (!client.connected()) {
      connected = false;
      Serial.println("Connecting to server...");
      if (client.connect(serverIP, serverPort)) {
          Serial.println("Connected to server");
          connected = true;
      } else {
          Serial.println("Connection failed");
          delay(1000);
          return;
      }
  }

  // Request data from ADXL345
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(DATAX0);
  Wire.endTransmission();
  
  Wire.requestFrom(ADXL345_ADDRESS, 6);
  if (Wire.available() == 6) {
    int16_t x = Wire.read() | (Wire.read() << 8);
    int16_t y = Wire.read() | (Wire.read() << 8);
    int16_t z = Wire.read() | (Wire.read() << 8);

    float x_g = x * 0.004; // Convert to g-force
    float y_g = y * 0.004;
    float z_g = z * 0.004;

    // Print accelerations
    // Serial.printf("X: %.2f g, Y: %.2f g, Z: %.2f g\n", x_g, y_g, z_g);

    // Check thresholds and send commands

    // flag for whether we did any change, if not stop
    bool fg = 0;
    if (x_g > thresholdX) {
      direction ="F";
      fg = 1;
    } else if (x_g < -thresholdX) {
      direction = "B";
      fg = 1;
    }

    if(!fg){
      if (y_g > thresholdY) {
        direction ="L";
      }else if (y_g < -thresholdY) {
        direction ="R";
      }else{
        direction = "S";
      }
    }
  }else{
    direction = "S";
  }

  sendCommandToCar(direction, speed, horn);

  delay(1000);
}

void sendCommandToCar(String direction, String speed, String horn) {
  if (!client.connected()) {
      Serial.println("Not connected to server");
      return;
  }
  String command = direction + speed + horn;
  // String command = "L" + speed + horn;
  Serial.println("Sending command: " +command);
  // client.print(direction);
  // client.print(speed);
  // client.print(horn);
  //client.connect(serverIP, serverPort);
  client.print(command + "\r\n"); // Send command with newline
}