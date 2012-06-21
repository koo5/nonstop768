#include <SPI.h>
#include <TimerOne.h>


//while true; do stty -F /dev/ttyACM0 115200&&netcat -p 768 -l > /dev/ttyACM0; sleep 0.1; done
//while true; do date "+%H%M%S%d.%m.%n";echo -e "\n"; sleep 1; done | nc 10.55 768

   //0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 


#include "font.xbm"

// http://overcode.yak.net






char text[9] = "10.55 99";
int lasttext=0; // a downward counter thats set high after a serial read
int te =0; // cursor

long period = 900;
int particle_count;
byte delete_trail = 1;
int clsbyte = 255;
byte back;
byte bri = 31;
const byte levels = 4;
const byte width = 4;//of display in bytes
byte leds[2][levels][width][16];
int mode = 0;











void led(byte x, byte y, byte b)
{
    if(x<0)return;
    if(x>=width*8)return;
    if(y<0)return;
    if(y>=16)return;
/*    if (b>1)
    {
	if ((x<4)||((x>15)&&(x<28)))
    	{
    		if(x<4)b++;
    		b=b/2;
 	}
    }*/
    for(byte l=0;l<levels;l++)
    {
	if (b > l)
            leds[back][l][x/8][y] &=~ (1<<(x%8));
    	else
    	    leds[back][l][x/8][y] |=  (1<<(x%8));
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
//    	if (delete_trail)led(ps[i].x,ps[i].y,clsbyte?0:255);

	bounce(i);
        ps[i].x+=ps[i].a;
	ps[i].y+=ps[i].b;

        led(ps[i].x,ps[i].y,bri);
    }
}


void cls(byte b)
{
    int i,j;
    for(i = 0; i < width*8; i++)
    	for(j = 0; j < 16; j++)
	    led(i,j,b);
}


void randomspeed(byte i,float div)
{
        ps[i].a=(random(2000)-1000)/div;
        ps[i].b=(random(2000)-1000)/div;
}

void randomspeeds(float div)
{
    for (int i=0;i<PS;i++)
    {
        ps[i].a=(random(2000)-1000)/div;
        ps[i].b=(random(2000)-1000)/div;
    }
}

void unified()
{
    for (int i=0;i<PS;i++)
    {
        ps[i].x=random(30);
        ps[i].y=random(15);
        ps[i].a=(float)(random(2)-1)/100.0;
        ps[i].b=(float)(random(2)-1)/100.0;
        
    }
}

void unify()
{
    for (int i=0;i<PS;i++)
    {
        ps[i].x=int(ps[i].x);
        ps[i].y=int(ps[i].y);
        ps[i].a=(float)(random(2)-1)/100.0;
        ps[i].b=(float)(random(2)-1)/100.0;
        
    }
}

void stop()
{
    for (int i=0;i<PS;i++)
    {
        ps[i].a=0;
        ps[i].b=0;
    }
}

void setupleds()
{
    cls(0);
    int i;
    for (i=0;i<PS;i++)
    {
    	randomspeed(i, 100000);
        ps[i].x=random(16);
        ps[i].y=random(16);
    }
}











void textin()
{

	if(Serial.available())
	{
		char in  =  Serial.read();
		static char lastchar;
		if (in != 10)
		{
		    text[te] = in;
		    if(++te>7)
			te = 0;
		}
		else
		{
			te = 0;
			if (lastchar == 10)
			{
			    int i;
			    for (i=0;i<8;i++)
				text[i]=' ';
			}
		}
		lastchar=in;
	}

}








const int A = A4;
const int B = A5;
const int ST =7;
const int OD =6;
const int RND = A3;

int r;
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
    Serial.begin(57600);
    randomSeed(analogRead(RND));
    SPI.setClockDivider(2);
    SPI.setBitOrder(LSBFIRST);
    SPI.begin();
    unified();
//    setupleds();
    Timer1.initialize(period);
    Timer1.attachInterrupt(setdrawflag);
}



byte row, level;
void draw()
{
        int brightness = bri;

	if(mode==0)
	{
		int i,j;
		for(i = 0; i < 4; i++)
		{
			if(brightness-->0)PORTD=0b00000000;
    
		        for(j = 0; j < 4; j++)
    			{

				if(brightness==0)PORTD=0b01000000;

				int y = row+j*4;
				
				if(brightness==1)PORTD=0b01000000;

				//	    byte cursor = (((y%8==7)   &&    (te/6==y/8)    &&    (te%6==i))    ?   ~lasttext    :    0xff);
				char ch  = text[i+y/8*4];

				if(brightness==2)PORTD=0b01000000;

				SPI.transfer(~ font_bits[ ch + (128*(y%8)) ]);


			}
		}

	}
	else
	{


    int i,j;
    for(i = 0; i < width; i++)
    {
	if(brightness-->0)PORTD=0b00000000;
        for(j = 0; j < 4; j++)
        {
	    if(brightness==0)PORTD=0b01000000;
	    int y = row+j*4;
	    if(brightness==1)PORTD=0b01000000;
	    SPI.transfer(leds[!back][level][i][y]);
	    if(brightness==2)PORTD=0b01000000;
	}
    }
    }
    PORTD=0b01000000;
    PORTD = 0b11000000;
    PORTC = row<<4;

    if(4==++row)
    {
	row = 0;
	level++;
	if(level == levels)
	    level = 0;
    }
}


int drawflag;

void setdrawflag()
{
    drawflag = 1;
}


void loop()
{
	if (drawflag)
	{
	    draw();
	    drawflag = 0;
	}
	
	if(mode==0)
	{
	
	textin();
	//snprintf(text, 9,"%08d",  millis()/1000);
	
	
	}
	else
	{
	memset(leds[back], clsbyte, levels*width*16);
	boing();
	back=!back;
	if(Serial.available())
	{
	    switch(Serial.read()){
		case '+':
		    bri++;
		    break;
		case '-':
		    bri--;
		    break;
		case 'r':
			if (period > 800)
			period-=100;
			Timer1.setPeriod(period);
		 	break;
		case 'a':
			period+=100;
			Timer1.setPeriod(period);
		 	break;
		case '1':
			randomspeeds(1000);
		 	break;
		case '2':
			randomspeeds(10000);
		 	break;
		case '3':
			randomspeeds(100000);
		 	break;
		case '5':
			unify();
		 	break;
		case 10:
			if (!clsbyte) clsbyte = 255;
			else clsbyte = 0;
		 	break;
		case 'o':
			stop();
		 	break;
	    }
	}
	}
	
}	

