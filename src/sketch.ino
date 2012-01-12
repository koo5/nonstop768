#include <SPI.h>



const int OE =A5;
const int B = A4;
const int A = A3;
const int ST =A2;



const int L = 3;
byte leds[L][6][16];
const int frames = (1<<L)-1;
byte la[frames];
byte rnd[6][16];
byte dirs[6][16];




int oo(int x)
{
    return x<12 || (x > 23 && x < 36);
}
int g(int x)
{
    return (x > 23 && x < 36);
}
int r(int x)
{
    return (x > 35);
}




void led(int x, int y, int b)
{
    int l;
    for(l=0;l<L;l++)
    {
	if (b >= L-l)
            leds[l][x/8][y] &=~ (1<<(x%8));
    	else
    	    leds[l][x/8][y] |=  (1<<(x%8));
    }
}


int getled(int x, int y)
{
    int l;
    for(l=0;l<L;l++)
	if (!(leds[l][x/8][y] & (1<<(x%8))))
	    return L-l;
    return 0;
}




void animate()
{
    static int i,j;
    led(i,j,  r(i));
    i = ++i;
    if(i > 47)
    {
	i = 0;
	j = ++j % 14;
    }
}



unsigned long fps;
void fps_bar()
{
    static byte i;
    static int bar;
    static unsigned long now, then, lastfps;
    led(i,15, (bar >= i));
    led(i,14, ((1l<<(47-i))&lastfps));
    i++;
    if(i>47)
    {
	bar = (fps*20)/((now = millis()) - then);
	lastfps = fps;
	then = now;
	fps = 0;
	i=0;
    };
}



typedef struct
{
    int a,b;
    int x,y;

} particle;
const int PS = 1;
particle ps[PS];


void wrap(int i)
{
        if(ps[i].x>47)
		ps[i].x=0;
        if(ps[i].x<0)
		ps[i].x=47;
        if(ps[i].y>15)
		ps[i].y=0;
        if(ps[i].y<0)
		ps[i].y=15;
}

void bounce(int i)
{
        if(ps[i].x>46)
		ps[i].a=-ps[i].a;
        if(ps[i].x<1)
		ps[i].a=-ps[i].a;
        if(ps[i].y>12)
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
        led(ps[i].x,ps[i].y,1+!oo(ps[i].x));
    }
}





void setup()
{
    pinMode(A,OUTPUT);
    pinMode(B,OUTPUT);
    pinMode(ST,OUTPUT);
    pinMode(OE,OUTPUT);
    SPI.begin();
    SPI.setBitOrder(LSBFIRST);
    SPI.setClockDivider(128);
    
    
    
    int i,j;
    	for(i = 0; i < 6*8; i++)
	{
	    int o = ( i > 40 );
    	    for(j = 0; j < 16; j++)
		led(i,j, 0);
	}


    pinMode(A0, INPUT);
    randomSeed(analogRead(A0));
    for (i=0;i<PS;i++)
    {
        ps[i].a=-2*random(2)+1;
        ps[i].b=-2+random(2)+1;
        ps[i].x=random(46)+1;
        ps[i].y=random(11)+1;
;
    }
    
    
    for(i=0;i<6;i++)
	for(j=0;j<16;j++)
	    rnd[i][j] = random(frames);
    
	int f;
	for(f=0;f<frames;f++)
	{
            int lap=0;
	    while(lap!=L-1)
            {
		    if(f & (1<<(lap/2)))
			break;
		    lap++;
            }
            la[f]=lap;
        }
}


int row,frame=1;
int animator;
void loop()
{
    int i,j,k=0;
    for(i = 0; i < 6; i++)
        for(j = 0; j < 4; j++)
        {
    	    int b = la[ ( rnd[i][j] + frame) % frames];
	    SPI.transfer(leds[b][i][row+j*4]);
	}

/*
    digitalWrite(ST,0);
    digitalWrite(OE,1);
    digitalWrite(ST,1);
    digitalWrite(A, row&1);
    digitalWrite(B, row&2);
    digitalWrite(OE,0);
*/

    PORTC |= 36;
    PORTC = 219 & (row<<3);
    
    
    fps++;


    if(row++ == 3)
    {
	row = 0;
        if (frame++ == (1<<L)-1)
        {
	    frame = 1;
	    boing();
	    fps_bar();
	}
    }
}



//	led(random(6*8),random(16),random(L));
