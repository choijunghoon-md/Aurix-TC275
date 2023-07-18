#include <SPI.h>

void setup (void)
{
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(SS, INPUT);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPCR |= _BV(SPE);
  SPCR &= ~_BV(MSTR);
  SPCR |= _BV(SPIE);
  Serial.begin(115200);
}

ISR (SPI_STC_vect)
{
  byte Confirm = SPDR;
  Serial.println(Confirm);
  SPDR = Confirm;
}
void loop (void)
{
}
