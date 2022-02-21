#include <Wire.h>

int addr = 0x58;// 0xB0 >>> 0x58

void setup() {
 Serial.begin(9600); /* begin serial for debug */
 Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
 Wire.setClock(100000);
 delay(500);
}

void loop() {
 Wire.beginTransmission(addr); /* begin with device address 8 */
 Wire.write("hello");
 Wire.endTransmission();    /* stop transmitting */
 delay(100);
 Wire.requestFrom(addr, 5);
 while(Wire.available()){
    char c = Wire.read();
    Serial.print(c);
 }
 Serial.println();
 delay(1000);
}
