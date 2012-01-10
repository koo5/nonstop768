#include <SPI.h>



const int OE = 10;
const int A = 8;
const int B = 9;
const int ST = 7;


byte leds[6][16];

void randomize()
{
/*    int i,j;
    for(i = 0; i < 6; i++)
    for(j = 0; j < 16; j++)
    leds[i][j]=random(256);
*/
    leds[random(6)][random(16)]=  (  leds[random(6)][random(16)]|(1<<random(8)))&~(1<<random(8));
}

void setup()
{
    pinMode(A,OUTPUT);
    pinMode(B,OUTPUT);
    pinMode(ST,OUTPUT);
    pinMode(OE,OUTPUT);
    SPI.begin();
}

int row;
int animator;
void loop()
{
    digitalWrite(OE,1);
    digitalWrite(A, row&1);
    digitalWrite(B, row&2);
    int i,j;
    for(i = 0; i < 6; i++)
    for(j = 0; j < 4; j++)
    SPI.transfer(~leds[i][row+j*4]);
    digitalWrite(ST,1);
    digitalWrite(ST,0);
    digitalWrite(OE,0);
    if(row++ > 2) row = 0;
    if(animator++ > 1)
    {
	animator = 0;
	randomize();
    }
    delay(1);
}
