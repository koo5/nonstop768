#include <SPI.h>



byte bri = 0;
const byte levels = 10;
const byte width = 4;//of display in bytes
byte leds[levels][width][16];

void led(int x, int y, int b)
{
    if(x<0)return;
    if(x>=width*8)return;
    if(y<0)return;
    if(y>=16)return;
    
    int l;
    for(l=0;l<levels;l++)
    {
	if (b > l)
            leds[l][x/8][y] &=~ (1<<(x%8));
    	else
    	    leds[l][x/8][y] |=  (1<<(x%8));
    }
}



typedef struct
{
    float a,b;
    float x,y;

} particle;


const int PS = 6;
particle ps[PS];


void bounce(int i)
{
        if(ps[i].x>width*8)
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


void cls(byte b)
{
    int i,j;
    for(i = 0; i < width*8; i++)
    	for(j = 0; j < 16; j++)
	    led(i,j,b);
}


void setupleds()
{
    cls(bri);
    int i;
    for (i=0;i<PS;i++)
    {
        ps[i].a=(random(2000)-1000)/10000.0;
        ps[i].b=(random(2000)-1000)/10000.0;
        ps[i].x=random(width*8);
        ps[i].y=random(16);
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
    randomSeed(10);
    SPI.setClockDivider(2);
    SPI.setBitOrder(LSBFIRST);
    SPI.begin();
    setupleds();
    Serial.begin(57600);
}


int level=1;
byte row,frame=1;
void loop()
{
    int i,j;
    int brightness = level;
    for(i = 0; i < width; i++)
    {
	if(brightness-->0)PORTD=0b00000000;
        for(j = 0; j < 4; j++)
        {
	    if(brightness==0)PORTD=0b01000000;
	    int y = row+j*4;
	    if(brightness==1)PORTD=0b01000000;
	    SPI.transfer(leds[level][i][y]);
	    PORTD=0b01000000;


	}
    }

    digitalWrite(ST, 0);
    PORTC = row<<4;
    digitalWrite(ST, 1);

    if (++frame >8)
    {
	boing();
	frame = 0;
	if(Serial.available())
	{
	    switch(Serial.read()){
		case '+':
		    bri++;
		    break;
		case '-':
		    bri--;
		    break;
	    }
	    cls(bri);
	    Serial.println(bri);
	}
    }
    
    if(4==++row)
    {
	row = 0;
	level++;
	if(level == levels)
	    level = 0;
    }
    
}


