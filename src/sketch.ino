#include <SPI.h>



const int OE = 10;
const int A = 8;
const int B = 9;
const int ST = 7;



const int L = 5;
byte leds[L][6][16];
const int frames = (1<<L)-1;
byte la[6][16][frames];

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
    	    led(i,j,  ( getled(i,j) + (dirs[i/8][j]&(1<<(i%8)) ? 1 : -1 )   ) );
}




typedef struct
{
    int a,b;
    int x,y;

} particle;


const int PS = 16;
particle ps[PS];


void boing()
{
    int i;
    for (i=0;i<PS;i++)
    {
    	led(ps[i].x,ps[i].y,0);
	ps[i].a = random(3)-1;;
        ps[i].x+=ps[i].a;
	ps[i].y+=ps[i].b;
        if(ps[i].x>47)
		ps[i].x=0;
        if(ps[i].x<0)
		ps[i].x=47;
        if(ps[i].y>15)
		ps[i].y=0;
        if(ps[i].y<0)
		ps[i].y=15;
	int o = (ps[i].x<12 || (ps[i].x > 23 && ps[i].x < 36));
        led(ps[i].x,ps[i].y,ps[i].y%L);
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
	    int o = (i<12 || (i > 23 && i < 36));
    	    for(j = 0; j < 16; j++)
		led(i,j, !o * j%L);
	}


    pinMode(A0, INPUT);
    randomSeed(analogRead(A0));
    for (i=0;i<PS;i++)
    {
        ps[i].b=-1;
        ps[i].x=random(48);
        ps[i].y=i;
    }
    
    
    for(i=0;i<6;i++)
    for(j=0;j<16;j++)
    {
	int off = random(8);
	int f;
	for(f=0;f<frames;f++)
	{
            int lap=0;
	    while(lap!=L-1)
            {
		    if((off+f)%frames & (1<<lap))
			break;
		    lap++;
            }

            la[i][j][f]=lap;
        }
    }
}


int row,frame=1;
int animator;
void loop()
{
    int i,j;
    for(i = 0; i < 6; i++)
        for(j = 0; j < 4; j++)
        {
    	    int a = la[i][row+j*4][frame];
	    SPI.transfer(leds[a][i][row+j*4]);
	}
    digitalWrite(ST,0);
    digitalWrite(OE,1);
    digitalWrite(ST,1);
    digitalWrite(A, row&1);
    digitalWrite(B, row&2);
    digitalWrite(OE,0);

//	randomize();
//	animate();
//	led(random(6*8),random(16),random(L));


    if(row++ == 3)
    {
	row = 0;
    
//    	    if(animator++==5)
    	    {
//		boing();
		animator=0;
	    }


        if (frame++ == (1<<L)-1)
        {
        

	    frame = 1;
	}

    
    }
}



