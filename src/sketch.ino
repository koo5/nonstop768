#include <SPI.h>



const int OE = 10;
const int A = 8;
const int B = 9;
const int ST = 7;


byte leds[6][16];

void randomize()
{
    int i,j;
    for(i = 0; i < 6; i++)
    for(j = 0; j < 16; j++)
    leds[i][j]=random(256);
}

void setup()
{
    SPI.begin();
}

int row;
int animator;
void loop()
{
    digitalWrite(OE,0);
    digitalWrite(A, row&1);
    digitalWrite(B, row&2);
    int i,j;
    for(i = 0; i < 6; i++)
    for(j = 0; j < 4; j++)
    SPI.transfer(leds[i][row+j*4]);
    digitalWrite(ST,0);
    digitalWrite(ST,1);
    digitalWrite(OE,1);
    if(row++ > 3) row = 0;
    if(!(animator++ % 1000)
	randomize();
    delay(1);
}
