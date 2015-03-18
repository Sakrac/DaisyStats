#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
name,value,color1,color2,petal,linear1,linear2,,
yuck,50 to 100,c00 to f80,000 to fff,75 to 100,255,,,
ick,10 to 50,00f to 08f,888 to ccc,1 to 10,255,,,
jock,5 to 25,cc1 to 446,000 to 888,1 to 10,255,255,,
*/

int randrange(int range)
{
	int r = rand();
	return (r+(r>>8))%range;
}

float randfloat()
{
	return (float)rand()/(float)RAND_MAX;
}

int randcol(int colbase, int offsmax)
{
	int r = ((colbase>>16)&0xff) + randrange(2 * offsmax) - offsmax;
	int g = ((colbase>>8)&0xff) + randrange(2 * offsmax) - offsmax;
	int b = (colbase&0xff) + randrange(2 * offsmax) - offsmax;

	if (r>255) r = 255;
	else if (r<0) r = 0;
	if (g>255) g = 255;
	else if (g<0) g = 0;
	if (b>255) b = 255;
	else if (b<0) b = 0;

	return (r<<16) + (g<<8) + b;
}


int main(int argc, char *argv[])
{
	FILE *f;
	if (argc>1 && (f = fopen(argv[1], "w"))) {
		srand ((unsigned int)time(NULL));
		int count = argc>2 ? atoi(argv[2]) : 3;
		fprintf(f, "value,color1,color2,petal,type\n");
		for (int l=0; l<count; l++) {
			float v0 = 0.95f * randfloat() + 0.05f;
			float v1 = (1.0f-v0) * randfloat() + v0;
			int c0 = randcol(0x808080, 0x80);
			int c1 = randcol(c0, 0x20);
			int c2 = randcol(0x808080, 0x80);
			int c3 = randcol(c2, 0x20);
			float p0 = randfloat()*0.95f+0.05f;
			float p1 = p0 + 0.25f*randfloat();
			if (p1<=0.001f) p1 =  0.001f;
			else if (p1>0.999f) p1 = 0.999f;
			fprintf(f, "%f to %f,%06x to %06x,%06x to %06x,%f to %f,%d\n", v0, v1, c0, c1, c2, c3, p0, p1, (randrange(23)&1) ? 8:4);
		}
		fclose(f);
	}
	return 0;
}