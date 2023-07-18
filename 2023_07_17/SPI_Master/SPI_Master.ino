#include<SPI.h>

char received=0;
byte Send=0;
int Dubge=0;

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  digitalWrite(SS,HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  Serial.begin(115200);
}

void spi_communication(void){
  digitalWrite(SS,LOW);
  received = SPI.transfer(Send);
  digitalWrite(SS,HIGH);
  Send=((float)analogRead(A0)*255)/(float)(687);
  Dubge=received;
  if(Dubge < 0){
    Dubge+=255;
    }
  Serial.println(Dubge);
}


void loop() {
  // put your main code here, to run repeatedly:
   spi_communication();
}
