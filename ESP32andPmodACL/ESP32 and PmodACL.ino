#include <Wire.h>

#define ADXL345_ADDRESS 0x1D

// Register addresses
#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31
#define DATAX0 0x32

#define I2C_SDA 21
#define I2C_SCL 22

void setup() {
  Serial.begin(115200);
  
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);
  
  // Wake up the ADXL345
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(POWER_CTL);
  Wire.write(0x08); // Enable measurement mode
  Wire.endTransmission();

  // Set data format (full resolution, ±16g range)
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(DATA_FORMAT);
  Wire.write(0x0B); // Full resolution, ±16g
  Wire.endTransmission();
}

void loop() {
  // Request 6 bytes of data (2 bytes for each axis)
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(DATAX0);
  Wire.endTransmission();
  
  Wire.requestFrom(ADXL345_ADDRESS, 6);
  
  // Read X, Y, Z acceleration values
  if (Wire.available() == 6) {
    int16_t x = Wire.read() | (Wire.read() << 8);
    int16_t y = Wire.read() | (Wire.read() << 8);
    int16_t z = Wire.read() | (Wire.read() << 8);
    
    // Convert to g-force (based on full resolution mode)
    float x_g = x * 0.004; // 4mg/LSB in full resolution
    float y_g = y * 0.004;
    float z_g = z * 0.004;
    
    // Print acceleration values
    Serial.print("Acceleration - X: ");
    Serial.print(x_g);
    Serial.print(" g, Y: ");
    Serial.print(y_g);
    Serial.print(" g, Z: ");
    Serial.print(z_g);
    Serial.println(" g");
  }
  
  // Small delay to prevent overwhelming the serial monitor
  delay(1000); // Update every 100ms
}