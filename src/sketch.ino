#include <SPI.h>



const int OE = 10;
const int A = 8;
const int B = 9;
const int ST = 7;



const int L = 5;
byte leds[L][6][16];
byte dirs[6][16];



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


void randomize()
{
    led(random(6*8),random(16),random(L));
    int x,y,d,b;
    d=dirs[x=random(6)][y=random(16)] & (1<<(b=random(8)));
    if(!d)
        dirs[x][y] |= 1<<b;
    else
        dirs[x][y] &= ~(1<<b);
}

void animate()
{
    int i,j;
    for(i = 0; i < 6*8; i++)
        for(j = 0; j < 16; j++)
    	    led(i,j,  ( getled(i,j) + (dirs[i/8][j]&(1<<i%8) ? 1 : -1 )   ) );
}

int a,b;
int x,y;
void boing()
{
    led(x,y,0);
    x=x+a;
    y=y+b;
    if(x==6*8-1)
	a=-1;
    if(x==0)
	a=1;
    if(y==15)
	b=-1;
    if(y==0)
	b=1;
    led(x,y,20);
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
    	    for(j = 0; j < 16; j++)
		led(i,j, i %7);

}



int row,frame=1;
int animator;
void loop()
{
    static int la;
    int i,j;
    for(i = 0; i < 6; i++)
        for(j = 0; j < 4; j++)
	    SPI.transfer( leds[la][i][row+j*4]);
    digitalWrite(ST,0);
    digitalWrite(OE,1);
    digitalWrite(ST,1);
    digitalWrite(A, row&1);
    digitalWrite(B, row&2);
    digitalWrite(OE,0);

    if(row++ == 3)
    {
	row = 0;
    
        if (frame++ == 63)
	    frame = 1;
	    
        la=0;
	while(la!=L)
        {
	    if(frame & (1<<la))
	    break;
	    la++;
        }

    
    }
    if(animator++ > 5000)
    {
	animator = 0;
	
//	randomize();
//	animate();
    }
//    if(!(animator%500))
//    	boing();
}


