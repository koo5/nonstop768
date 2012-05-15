#include <SPI.h>






byte leds[8][16];
void led(int x, int y, int b)
{
    if (b >= 1)
        leds[x/8][y] &=~ (1<<(x%8));
    else
    	leds[x/8][y] |=  (1<<(x%8));
}



typedef struct
{
    float a,b;
    float x,y;

} particle;


const int PS = 3*5;
particle ps[PS];


void bounce(int i)
{
        if(ps[i].x>48)
		ps[i].a=-ps[i].a;
        if(ps[i].x<1)
		ps[i].a=-ps[i].a;

//		Y:
        if(ps[i].y>15)
		ps[i].b=-ps[i].b;
        if(ps[i].y<1)
		ps[i].b=-ps[i].b;
}



void boing()
{
    int i;
    for (i=0;i<PS;i++)
    {
    	led(ps[i].x,ps[i].y,0);
	bounce(i);
        ps[i].x+=ps[i].a;
	ps[i].y+=ps[i].b;
        led(ps[i].x,ps[i].y,100);
    }
}



void setupleds()
{
    int i,j;
    for(i = 0; i < 6*8; i++)
    	for(j = 0; j < 16; j++)
	    led(i,j, 0);

    for (i=0;i<PS;i++)
    {
        ps[i].a=(random(2000)-1000)/10000.0;
        ps[i].b=(random(2000)-1000)/10000.0;
        ps[i].x=random(36)+10;
        ps[i].y=random(11)+1;
    }
}







const int A = A4;
const int B = A5;
const int ST =7;
const int OD =6;
const int RND = A3;


void setup()
{
    pinMode(A2,OUTPUT);
    digitalWrite(A2, 1);
    pinMode(A,OUTPUT);
    pinMode(B,OUTPUT);
    pinMode(SCK,OUTPUT);
    pinMode(MOSI,OUTPUT);
    pinMode(ST,OUTPUT);
    pinMode(OD,OUTPUT);
    digitalWrite(OD, 1);
    pinMode(RND, INPUT);
    randomSeed(analogRead(RND));
    SPI.setClockDivider(2);
    SPI.setBitOrder(LSBFIRST);
    SPI.begin();
    setupleds();
    Serial.begin(57600);
}

char brightness;
byte row,frame=1;
void loop()
{
    int i,j;

    SPI.transfer(255);
    int dead = 0;

    for(i = 0; i < 6; i++)
    {
	PORTD=0b00000000;
        for(j = (!dead); j < 4; j++)
        {
	    dead++;
	    int y = row+j*4;
	    PORTD=0b01000000;

	    SPI.transfer(leds[i][y]);

	}
    }

    digitalWrite(ST, 0);
    PORTC = row<<4;
    digitalWrite(ST, 1);
    brightness = 6*((analogRead(RND)-100)/5);
    

    if (++frame >15)
    {
	boing();
	frame = 0;
	Serial.println((int)brightness);
    }
    
    if(4==++row)
    {
	row = 0;
    }
    
}


