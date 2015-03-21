/*
	Copyright 2015 Carl-Henrik Skårstedt. All rights reserved.

	https://github.com/sakrac/daisystats
*/
// standard stuff
#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <ctype.h>

// STB awesomeness
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_image.h"			// https://github.com/nothings/stb/blob/master/stb_image.h
#include "stb_image_write.h"	// https://github.com/nothings/stb/blob/master/stb_image_write.h
#include "stb_truetype.h"		// https://github.com/nothings/stb/blob/master/stb_truetype.h

// Support for Windows shenanigans
#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define strtoll _strtoi64
#define snprintf sprintf_s
#else
#include <strings.h>
#endif
#ifdef WIN32
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#endif

// constants
#define invsqrt2 0.70710678118654752440084436210485
#define pi_dbl 3.1415926535897932384626433832795

const char Instructions[] = {
	"\n"
	"command line arguments:\n"
	"\n"
	"daisystats [a=<num>] [s=<num>] [f=<shape>] [o=<condition>] [p=<csv file>] [r=<num>] [d=<csv file>] [b=<color>] [input.csv] output.png \n"
	"\n"
	" All command line arguments can also be rows in the top of input.csv file\n"
	" If the only command line argument is a csv file a variety of guesses for\n"
	"   command line arguments will be made based on the contents of that file.\n"
	"\n"
	" a/aspect=<num> : aspect ratio for round or rect, divide width by height\n"
	" b/background=<color>/<image file> : set background color or image\n"
	" c/color=<color> : set text color\n"
	" d/data=<csv file> : Load in a csv file that has values to represent\n"
	" f/font=<font>: The font for legend, font should have a .ttf extension.\n"
	" l/legend=<height> : add a legend of the data names at the bottom\n"
	" l/legend=inside : show the legend inside the flowers instead of the bottom\n"
	" n/name_color=<color> : Use a separate color for names than the title\n"
	" m/make=<shape> : Make a shape. Shape is one of: most, round, rect, first, last\n"
	" o/order=<condition> : Packing order. (orig, large, small, shuffle, name)\n"
	" p/preset=<csv file> : Load in a csv file of preset flowers\n"
	" r/random=<num> : Create <num> random sized flowers, uses presets\n"
	" s/size=<num> : Make the result fit within this size (max width or height)\n"
	" t/title=<size>:<name> : Add a title on the top of the page."
	"\n"
	" Note about colors:\n"
	"   The color parameter can be 0-15 for c64 palette, 000-fff for amiga, 0000-ffff amiga+alpha,\n"
	"   000000-ffffff for web or 00000000-ffffffff for web+alpha or color name. Valid color\n"
	"   names include old classics such as transparent, black, white, orange, blue, azure,\n"
	"   teal, jade, green, cyan, turquoise, taupe, indigo, taupe, emerald, coffee, chocolate,\n"
	"   aqua, maroon, burgundy, purple, olive, gray, brown, lavender, lime, lilac,\n"
	"   periwinkle, bronze, pear, tan, orchid, crimson, cerise, ruby, violet, beige, coral,\n"
	"   red, scarlet, pink, yellow, white.\n"
	"\n"
	"\n"
	"\n"
	"CSV (excel, pages, google sheets, etc.) format:\n"
	"\n"
	" any of the following strings in the top row will define that column to define a range of that property:\n"
	"   value: the value (circle area) of data/Users/Carl-Henrik/Documents/flower_presets.csv\n"
	"   name: optional value name\n"
	"   center: proportion of diameter of flower that is the center of the flower in percent (~15-50)\n"
	"   angle: angle in degrees\n"
	"   petal: width of petals as percent, 100 is widest, 0 is thinnest\n"
	"   color1: color of petals\n"
	"   color2: color of center\n"
	"   linear1: linearity of petal color randomization in %, 0 means box space 100 means straight line\n"
	"   linear2: linearity of center color randomization in %\n"
	"   flat: petal ends rounded at 0.0 and flat at 1.0\n"
	"   type: number of petals in flower, between 3 and 32\n"
	"   count: create this many copies of this entry\n"
	"\n"
	" To export excel, pages or any other spreadsheet software's proprietary\n"
	"   data format to CSV, look in the file menu for an option to \"Export\"\n"
	"   \"Publish\" or \"Download As\" to locate the CSV option. All spreadsheet\n"
	"   software can open CSV files.\n"
	"\n"
	" Any value can be defined as a range and flowers will be created randomly within that range. By\n"
	" default the full range is used but can be overridden by any of the columns named above.\n"
	" To declare the range just add 'to' within the cell, for example: 'white to black' means all colors\n"
	" or '15 to 50' means any number between 15 and 50.\n"
	"\n"
	" For colors in spreadsheets, see command line arguments\n"
	"\n"
	"Latest Version / More Info: https://github.com/sakrac/daisystats\n"
};


//
//
// ENUMS AND NAMES
//
//


enum arguments {
	A_ASPECT,
	A_BACKGROUND,
    A_COLOR,
	A_DATA,
	A_FONT,
	A_LEGEND,
    A_NAME_COLOR,
	A_MAKE,
	A_ORDER,
	A_PRESET,
	A_RANDOM,
	A_SIZE,
	A_TITLE,

	A_COUNT
};

const char *cmd_args[] = {
	"aspect",
	"background",
    "color",
	"data",
	"font",
	"legend",
    "name_color",
	"make",
	"order",
	"preset",
	"random",
	"size",
	"title"
};

enum fit {
	FIT_ROUND,
	FIT_RECT,
	FIT_FIRST,
	FIT_LAST,
	FIT_MOST,

	FIT_COUNT
};

const char *fit_name[] = {
	"round",
	"rect",
	"first",
	"last",
	"most"
};

enum sort {
	SORT_ORIG,
	SORT_LARGE,
	SORT_SMALL,
	SORT_SHUFFLE,
	SORT_NAME,

	SORT_COUNT
};

const char *sort_name[] = {
	"orig",
	"large",
	"small",
	"shuffle",
	"name"
};

enum ColumnIndex {
	CLN_VALUE,
	CLN_NAME,
	CLN_CENTER,
	CLN_ANGLE,
	CLN_PETAL,
	CLN_COLPETAL,
	CLN_COLCTR,
	CLN_LINPETAL,
	CLN_LINCTR,
	CLN_FLAT,
	CLN_TYPE,
	CLN_LENGTH,
	CLN_COUNT,

	CLN_ENTRIES
};

const char *ColumnName[CLN_ENTRIES] = {
	"value",
	"name",
	"center",
	"angle",
	"petal",
	"color1",
	"color2",
	"linear1",
	"linear2",
	"flat",
	"type",
	"length",
	"count"
};

typedef unsigned char u8;
typedef unsigned int u32;
struct color { u8 r, g, b, a; };

static const color c64[] = {
	{0, 0, 0, 255},
	{255, 255, 255, 255},
	{136, 0, 0, 255},
	{170, 255, 238, 255},
	{204, 68, 204, 255},
	{0, 204, 85, 255},
	{0, 0, 170, 255},
	{238, 238, 119, 255},
	{221, 136, 85, 255},
	{102, 68, 0, 255},
	{255, 119, 119, 255},
	{51, 51, 51, 255},
	{119, 119, 119, 255},
	{170, 255, 102, 255},
	{0, 136, 255, 255},
	{187, 187, 187, 255}
};

static struct { const char *name; color col; } colornames[] = {
	{ "tra", { 0,0,0,0 } },
	{ "Bla", { 0, 0, 0, 255} },
	{ "Blu", { 0, 0, 255, 255} },
	{ "Azu", { 0, 127, 255, 255} },
	{ "Teal", { 0, 128,	128, 255} },
	{ "Jade", { 0, 168,	107, 255} },
	{ "Gre", { 0, 255,	0, 255} },
	{ "Cyan", { 0, 255,	255, 255} },
	{ "Turq", {  64, 224, 208, 255} },
	{ "Taupe", { 72, 60, 50, 255} },
	{ "Ind", { 75, 0, 130, 255} },
	{ "Eme", { 80, 200, 120, 255} },
	{ "Cof", { 111, 78, 55, 255} },
	{ "Choc", { 123, 63, 0, 255} },
	{ "Aqua", { 127, 255, 212, 255} },
	{ "Mar", { 128, 0, 0, 255} },
	{ "Burg", { 128, 0, 32, 255} },
	{ "Purpl", { 128, 0, 128, 255} },
	{ "Oliv", { 128, 128, 0, 255} },
	{ "Gr", { 128, 128, 128, 255} },
	{ "Bro", { 150, 75, 0, 255} },
	{ "Lav", { 181, 126, 220, 255} },
	{ "Lime", { 191, 255, 0, 255} },
	{ "Lil", { 200, 162, 200, 255} },
	{ "Periw", { 204, 204, 255, 255} },
	{ "Bronze", { 205, 127, 50, 255} },
	{ "Pear", { 209, 226, 49, 255} },
	{ "Tan", { 210, 191, 140, 255} },
	{ "Orc", { 218, 112, 214, 255} },
	{ "Crim", { 220, 20, 60, 255} },
	{ "Cer", { 222, 49, 99, 255} },
	{ "Ruby", { 224, 17, 95, 255} },
	{ "Viol", { 238, 130, 238, 255} },
	{ "Bei", { 245, 245, 220, 255} },
	{ "Cor", { 248, 131, 121, 255} },
	{ "Red", { 255, 0, 0, 255} },
	{ "Scar", { 255, 36, 0, 255} },
	{ "Orang", { 255, 165, 0, 255} },
	{ "Pink", { 255, 192, 203, 255} },
	{ "Yel", { 255, 255, 0, 255} },
	{ "Whi", { 255, 255, 255, 255} }
};
static const int num_colornames = sizeof(colornames) / sizeof(colornames[0]);

#define font_file "GEORGIA.TTF"
#ifdef WIN32
#define font_folder "C:\\Windows\\Fonts\\"
#else
#define font_folder "/Library/Fonts/"
#endif


//
//
// RENDER FLOWERS
//
//

// get the radius value for x, y and k
inline double petval4(double x, double y, double k)
{
	static const double ep = 1e-12;
	double ax = fabs(x);
	double ay = fabs(y);

	return x*x+y*y+k*(x*x/(ay<ep?ep:ay)+y*y/(ax<ep?ep:ax));
}

void drawnpetal(unsigned int *bitmap, int bitmap_width, double x, double y, double r, double c, double a, double k, double f, unsigned int petcol, unsigned int ctrcol, int petals)
{
	double orc = r-c;
	double tip_angle = (pi_dbl/4) * (2.0-f)/2.0;
	double pe = petval4(cos(tip_angle), sin(tip_angle), k);
	double ce = c*c, fe = r*r;
	double ps = (double)petals/4.0;

	// get image center point
	int cx = (int)x;
	int cy = (int)y;

	// get fraction of pixel
	x -= (double)cx;
	y -= (double)cy;

	int ir = (int)(2.0*(r+1.0));	// integer radius with buffer

	bitmap += (cy-ir)*bitmap_width + cx;
	for (int v=-ir; v<=ir; v++) {
		int w = int(sqrt(r*r+1.0-double(v*v)))+1;
		unsigned int *draw = bitmap - w;
		bitmap += bitmap_width;
		for (int h=-w; h<=w; h++) {
			double vx = double(h)+x;
			double vy = double(v)+y;
			double rp = (vx*vx+vy*vy);
			if (rp<fe) {
				if (rp<ce)
					*draw++ = ctrcol;
				else {
					double ap = atan2(vx, vy) * ps + a;
					rp = (sqrt(rp) - c)/orc;
					if (petval4(rp*cos(ap), rp*sin(ap), k) < pe)
						*draw = petcol;
					draw++;
				}
			}
		}
	}
}


//
//
// FILE READ
//
//


char* ReadFile(const char *filename, size_t &size, int padding=0)
{
	if (FILE *f=fopen(filename, "rb")) {
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		fseek(f, 0, SEEK_SET);
		if (char *data = (char*)malloc(size+padding)) {
			fread(data, size, 1, f);
			fclose(f);
			return data;
		}
		fclose(f);
	}
	return nullptr;
}




//
//
// RENDER TEXT
//
//



static stbtt_fontinfo font;
static const void *ttf_file = nullptr; // for shutdown cleanup
static unsigned char *glyph_buf = nullptr;
static unsigned int glyph_buf_wid = 0, glyph_buf_height = 0;

const char *FontFolder(char* buf, size_t buf_size)
{
#ifdef WIN32
	if (int len = GetEnvironmentVariableA("WINDIR", buf, (DWORD)buf_size)) {
		if ((buf_size-len)>=8) {
			memcpy(buf+len, "\\fonts\\", 8);
			return buf;
		}
	}
#endif
	return font_folder;
}

bool InitFont(const char *fontname)
{
	size_t s;
	char filename_buf[512], path_buf[512];
	char *ttf_data = ReadFile(fontname, s);
	if (!ttf_data) {
		snprintf(filename_buf, sizeof(filename_buf), "%s.ttf",
				 fontname);
		ttf_data = ReadFile(filename_buf, s);
		if (!ttf_data) {
			snprintf(filename_buf, sizeof(filename_buf), "%s%s",
					 FontFolder(path_buf, sizeof(path_buf)), fontname);
			ttf_data = ReadFile(filename_buf, s);
			if (!ttf_data) {
				snprintf(filename_buf, sizeof(filename_buf), "%s%s.ttf",
						 FontFolder(path_buf, sizeof(path_buf)), fontname);
				ttf_data = ReadFile(filename_buf, s);
			}
		}
	}

	if (ttf_data) {
		ttf_file = ttf_data;
		if (!stbtt_InitFont(&font, (unsigned char*)ttf_data, 0)) {
			free(ttf_data);
			return false;
		}
		printf("Loaded Font \"%s\"\n", fontname);
		return true;
	}
	return false;
}

void ShutdownFont()
{
	if (ttf_file) {
		free((void*)ttf_file);
		ttf_file = nullptr;
	}
	if (glyph_buf) {
		free((void*)glyph_buf);
		glyph_buf = nullptr;
	}
}

float FontSizeScale(float height)
{
	return stbtt_ScaleForPixelHeight(&font, height);
}

int FontBaseline()
{
	int ascent, descent;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, 0);
	return ascent;
}

int FontCenterHgt()
{
	int ascent, descent;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, 0);
	return (ascent+descent)/2;
}

#define TTY_GUARD 2048
void DrawCodepointAt(int codepoint, double x, double y, float scale, color col, unsigned int *bm_trg, int wid, int hgt)
{
	// cache the memory block for drawing characters to reduce allocs
	int bx0, by0, bx1, by1;
	stbtt_GetCodepointBitmapBoxSubpixel(&font, codepoint, scale, scale, (float)(x - floor(x)), (float)(y - floor(y)), &bx0, &by0, &bx1, &by1);

	int prev_size = glyph_buf_wid * glyph_buf_height;
	int new_size = (bx1 - bx0) * (by1 - by0);
	if (!glyph_buf || new_size > prev_size) {
		if (glyph_buf)
			free(glyph_buf);
		glyph_buf = (unsigned char*)malloc(TTY_GUARD * 2 + new_size * sizeof(unsigned int));
		glyph_buf_wid = bx1 - bx0;
		glyph_buf_height = by1 - by0;
	}
	int wc=bx1-bx0, hc=by1-by0, ox=bx0, oy=by0;

	stbtt_MakeCodepointBitmapSubpixel(&font, glyph_buf + TTY_GUARD, wc, hc, wc, scale, scale, (float)(x - floor(x)), (float)(y - floor(y)), codepoint);
	unsigned char *bm_char = glyph_buf + TTY_GUARD;

    int ix = ox + (int)x, iy = oy + (int)y;
	int w = (ix+wc)<wid ? wc : (wid-ix), h = (iy+hc)<hgt ? hc : (hgt-iy);

	if (ix>=wid || iy>=hgt || (ix+wc)<=0 || (iy+hc)<=0)
		return; // all outside

	if (ix<0) { w += ix; bm_char -= ix; ix = 0; }
	if (iy<0) { h += iy; bm_char -= iy*wc; iy = 0; }

	bm_trg += iy * wid + ix;
	if (unsigned char *bm_cr = bm_char) {
		unsigned int c32 = *(unsigned int*)&col;
		for (int dy=0; dy<h; dy++) {
			for (int dx=0; dx<w; dx++) {
				unsigned int c = (unsigned int)(*bm_cr++) * col.a / 255;
				if (c) {
					if (c==255)
						*bm_trg = c32;
					else {
						unsigned int d = 255-c;
						color orig = *(color*)bm_trg;
						color cmix = {
							(u8)(((u32)orig.r*d+(u32)col.r*c)/255),
							(u8)(((u32)orig.g*d+(u32)col.g*c)/255),
							(u8)(((u32)orig.b*d+(u32)col.b*c)/255),
							orig.a };
						*bm_trg = *(unsigned int*)&cmix;
					}
				}
				bm_trg++;
			}
			bm_trg += wid-w;
			bm_cr += wc-w;
		}
	}
}

void DrawTextAt(const unsigned char *utf8, float scale, double x, double y, color col, unsigned int *bm_trg, int wid, int hgt)
{
	int ascent, advance, lsb;
	stbtt_GetFontVMetrics(&font, &ascent,0,0);
	int prevcode = 0;
	for (;;) {
		int code = *utf8++;
		if (code>=0xc0) { int mask = 0x40; code &= 0x7f;
			for (;code&mask;mask<<=5) {
				int n = *utf8++; if ((n&0xc0)!=0x80) { code=0; break; }
				code = ((code&~mask)<<6) | (n&0x3f);
			}
		}
		if (!code) break;
		if (prevcode)
			x += scale * stbtt_GetCodepointKernAdvance(&font, prevcode, code);
		DrawCodepointAt(code, x, y, scale, col, bm_trg, wid, hgt);
		stbtt_GetCodepointHMetrics(&font, code, &advance, &lsb);
		x += scale * advance;
	}
}

struct textspace { int minx, maxx, miny, maxy; };
textspace GetTextSpace(const unsigned char *utf8)
{
	int advance, lsb, prevcode = 0;
	int xp = 0, yp = 0;
	int minx = 0x7fffffff, miny = 0x7fffffff, maxx = -0x7fffffff, maxy = -0x7fffffff;

	for (;;) {
		int code = *utf8++;
		if (code>=0xc0) { int mask = 0x40; code &= 0x7f;
			for (;code&mask;mask<<=5) {
				int n = *utf8++; if ((n&0xc0)!=0x80) { code=0; break; }
				code = ((code&~mask)<<6) | (n&0x3f);
			}
		}
		if (!code) break;
		int ix0, iy0, ix1, iy1;
		stbtt_GetCodepointBitmapBox(&font, code, 1.0f, 1.0f, &ix0, &iy0, &ix1, &iy1);
		if (minx>(xp+ix0)) minx = xp+ix0;
		if (miny>(yp+iy0)) miny = yp+iy0;
		if (maxx<(xp+ix1)) maxx = xp+ix1;
		if (maxy<(yp+iy1)) maxy = yp+iy1;
		if (prevcode)
			xp += stbtt_GetCodepointKernAdvance(&font, prevcode, code);
		stbtt_GetCodepointHMetrics(&font, code, &advance, &lsb);
		xp += advance;
		prevcode = code;
	}
	textspace ret = { minx, maxx, miny, maxy };
	return ret;

}


//
//
// RANDOM FOR TYPES
//
//


double dblrand()
{
	return double(rand()) / double(RAND_MAX);
}

unsigned char byterand(unsigned char low, unsigned char high)
{
	unsigned int li = low;
	unsigned int lh = high;
	unsigned int r = rand()%65535;
	return r * (li + lh + 32768) / 65535;
}

color colrand(color low, color high, u8 linear)
{
	unsigned int rr = rand()%255;
	unsigned int rg = rand()%255;
	unsigned int rb = rand()%255;
	unsigned int ra = rand()%255;

	// linear = 0 => no influence
	// linear = 255 => all the same
	unsigned int rs = (rr+rg+rb+ra)*linear/4;
	rr = (rs + rr*(255-linear))/255;
	rg = (rs + rg*(255-linear))/255;
	rb = (rs + rb*(255-linear))/255;
	ra = (rs + ra*(255-linear))/255;

	color ret = {
		(u8)(((rr * int(high.r-low.r) + 127) / 255) + low.r),
		(u8)(((rg * int(high.g-low.g) + 127) / 255) + low.g),
		(u8)(((rb * int(high.b-low.b) + 127) / 255) + low.b),
		(u8)(((ra * int(high.a-low.a) + 127) / 255) + low.a)};
	return ret;
}

//
//
// FLOWER SORTING AND PACKING
//
//


struct flower {
	double x, y, c, r, a, k, f;
	const char *name, *value;
	color col_pet, col_ctr;
	int type;
	u8 lin_pet, lin_ctr;	// 0 = rgb cube interp, 255 = straight
};

struct flower_pair {
    int first, second;
};

static flower default_low = {
	-10000.0, -10000.0, 0.15, 1.0, 0.0, 0.0001, 0.0, nullptr, nullptr,
	{0x40,0x40,0x40,0xff}, {0,0,0,0xff}, 3, 0, 0
};

static flower default_high = {
	10000.0, 10000.0, 0.4, 32.0, 6.24, 0.5, 0.0, nullptr, nullptr,
	{0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, 18, 0, 0
};

static int flower_larger(const void* p1, const void* p2)
{
	return ((const flower*)p1)->r > ((const flower*)p2)->r ? -1 : 1;
}

static int flower_smaller(const void* p1, const void* p2)
{
	return ((const flower*)p1)->r < ((const flower*)p2)->r ? -1 : 1;
}

static int flower_name(const void* p1, const void* p2)
{
	return p1==nullptr ? (p2==nullptr ? 0 : -1) : (p2==nullptr ? 1 : strcasecmp((const char*)p1, (const char*)p2));
}

void reorder(flower *flowers, int count, sort order)
{
	if (!count)
		return;

	if (order==SORT_SHUFFLE) {
		for (int n=1; n<count; n++) {
			int r = rand()%n;
			if (r!=n) {
				flower t = flowers[n];
				memmove(flowers+r+1, flowers+r, sizeof(flower) * (n-r));
				flowers[r] = t;
			}
		}
	} else if (order!=SORT_ORIG)
		qsort(flowers, count, sizeof(flower), order==SORT_LARGE ? flower_larger : (order==SORT_NAME ? flower_name : flower_smaller));
}

bool place_next_to(flower *flowers, flower *f, flower *f_first, flower *f_second, double &best_dist, double &x, double &y, double aspect, fit shape)
{
	double x0 = f_first->x;
	double y0 = f_first->y;
	double x1 = f_second->x;
	double y1 = f_second->y;
	double dx = x1-x0;
	double dy = y1-y0;
	double h2 = dx*dx+dy*dy;
	double h = sqrt(h2);
	double r0 = f_first->r+f->r;
	double r1 = f_second->r+f->r;
	double h0 = (h2+r0*r0-r1*r1)/(2.0*h);
	double w = r0*r0-h0*h0;
	if (w>=0.0) {
		w = sqrt(w);
		for (int s=0; s<2; s++) {
			double sign = s ? 1.0 : -1.0;
			double tx = x0+(h0*dx + sign*w*dy)/h;
			double ty = y0+(h0*dy - sign*w*dx)/h;
			double ox = tx-flowers->x;
			double oy = ty-flowers->y;
			double td = shape==FIT_RECT ?
			((fabs(oy)*aspect)>fabs(ox) ? fabs(oy)*aspect : fabs(ox)) :
			ox*ox+aspect*aspect*oy*oy;
			if (td<best_dist) {
				bool blocked = false;
				for (flower *c=flowers; c<f; c++) {
					if (c!=f_first && c!=f_second) {
						ox = c->x-tx;
						oy = c->y-ty;
						double ro = c->r+f->r;
						if ((ox*ox+oy*oy)<(ro*ro)) {
							blocked = true;
							break;
						}
					}
				}
				if (!blocked) {
					best_dist = td;
					x = tx;
					y = ty;
					return true;
				}
			}
		}
	}
	return false;
}

inline double sqr(double x) { return x*x; }

void pack_flowers(flower *flowers, int count, fit shape, double aspect)
{
	if (!count)
		return;

	int max_pairs = count*count, num_pairs = 0;
	flower_pair *prs = (flower_pair*)malloc(sizeof(flower_pair)*max_pairs);

	double x = 0.0;
	double y = 0.0;

	int n = 0;
	for (flower* f=flowers; f<(flowers+count); f++) {

		double r = f->r;

		if (n==1) {
			double a = dblrand()*3.14129654*2.0;	// pick a random angle and place there
			double rs = r + flowers->r;
			x = flowers->x + cos(a) * rs;
			y = flowers->y + sin(a) * rs;
			prs->first = 0;
			prs->second = 1;
			num_pairs = 1;
		} else if (n) {
			double best_dist = DBL_MAX;
			int found = 0;
			int best_pair = -1;
			if (shape==FIT_MOST) {
				for (flower* f2=flowers; f2<f; f2++) {
					for (flower *f3=flowers; f3<f2; f3++) {
						double r3 = f->r + f2->r + f3->r;
						double d2 = sqr(f3->x-f2->x) + sqr(f3->y-f2->y);
						if (d2 < sqr(r3)) {
							if (place_next_to(flowers, f, f2, f3, best_dist, x, y, aspect, shape)) {
								found++;
							}
						}
					}
				}
			} else {
				for (int pi=0; pi<num_pairs; pi++) {
					int p = shape==FIT_LAST ? num_pairs-1-pi : pi;
					const flower_pair &pair = prs[p];
					if (place_next_to(flowers, f, &flowers[pair.first], &flowers[pair.second],
									  best_dist, x, y, aspect, shape)) {
						found++;
						best_pair = p;
					}
					if ((shape==FIT_FIRST || shape==FIT_LAST) && found)
						break;
				}
				if ((num_pairs+1)<max_pairs) {
					if (found) {
						prs[num_pairs].first = prs[best_pair].first;
						prs[num_pairs].second = n;
						num_pairs++;
						prs[num_pairs].first = prs[best_pair].second;
						prs[num_pairs].second = n;
						num_pairs++;
					} else {
						// FAIL! Try all prior instead of just the pairs..
						for (int first=n-1; first>0; --first) {
							for (int second=first-1; second>=0; --second) {
								if (place_next_to(flowers, f, &flowers[first], &flowers[second],
												  best_dist, x, y, aspect, shape)) {
									prs[num_pairs].first = first;
									prs[num_pairs].second = n;
									num_pairs++;
									prs[num_pairs].first = second;
									prs[num_pairs].second = n;
									num_pairs++;
									found++;
									break;
								}
							}
						}
					}
				}
			}
		}
		f->x = x;
		f->y = y;
		n++;
	}
	free(prs);
}

// create a flower in a random range
void initflower(flower *f, const flower &low, const flower &high)
{
	f->x = 0.0;
	f->y = 0.0;
	f->r = dblrand()*(high.r-low.r)+low.r;
	f->c = dblrand()*(high.c-low.c)+low.c;
	f->a = dblrand()*(high.a-low.a)+low.a;
	f->k = dblrand()*(high.k-low.k)+low.k;
	f->f = dblrand()*(high.f-low.f)+low.f;
	f->name = low.name;
	f->value = low.value;
	f->col_ctr = colrand(low.col_ctr, high.col_ctr, low.lin_ctr);
	f->col_pet = colrand(low.col_pet, high.col_pet, low.lin_pet);
	f->type = low.type;
	if (high.type>low.type)
		 f->type += ((rand()*113)>>8) % (high.type-low.type);
}


//
//
// SIMPLE TESTS
//
//

int isNumeric(const char *s)
{
	if (!s || *s==0)
		return 0;
	while (*s==' ' || *s=='\t') s++;
	int digits = 0;
	while (char c = *s++) {
		if ((c>='0' && c<='9') ||
			(c>='a' && c<='f') ||
			(c>='A' && c<='F'))
			digits++;
		else if (c!=' ' && c!='\t' &&
				 c!='.' && c!='+' &&
				 c!='-' && c!='x')
			return digits;
	}
	return 1;
}

int stringIsText(const char *str)
{
	if (isNumeric(str)>=2)
		return 0;
	int textScore = 0;
	int length = 0;
	while (char c = *str++) {
		if ((c>='A' && c<='Z') || (c>='a' && c<='z'))
			textScore++;
		length++;
	}
	return length ? (textScore * 2) / length : 0;
}

int columnTextScore(const char **pCells, int col, int columns, int rows)
{
	int total = 0;
	for (int r=0; r<rows; r++)
		total += stringIsText(pCells[r*columns+col]);
	return total;
}

int rowIsMeaningful(const char **pCells, int columns, int value_index, int count_index)
{
	bool empty = true;
	for (int c=0; c<columns; c++) {
		const char *cell = pCells[c];
		if (cell && *cell!=0) empty = false;
	}
	if (!empty && (value_index>=0 && isNumeric(pCells[value_index]))) {
		if (count_index>=0) {
			int n = atoi(pCells[count_index]);
			if (n>1)
				return n;
		}
		return 1;
	}
	return 0;
}

bool isOneOf(int value, int *set, int set_size)
{
	for (int i=0; i<set_size; i++)
		if (value==set[i])
			return true;
	return false;
}

bool notOneOf(int value, int *set, int set_size)
{
	for (int i=0; i<set_size; i++)
		if (value!=set[i])
			return true;
	return false;
}


bool isHexOrDec(const char *val)
{
	while (*val==' ') val++;
	while (unsigned char c = (unsigned char)*val++) {
		if (c==' ')
			break;
		if (!(c>='0' && c<='9') && !(c>='a' && c<='f') && !(c>='A' && c<='F'))
			return false;
	}
	return true;
}

const char* strlastchar(const char *str, char t)
{
	if (!str)
		return nullptr;
	const char *end = str + strlen(str);
	while (end>=str && *end!=t) --end;
	if (end<str)
		return nullptr;
	return end;
}

const char* strlastchar(const char *str, char t, char s)
{
	if (!str)
		return nullptr;
	const char *end = str + strlen(str);
	while (end>=str && *end!=t && *end!=s) --end;
	if (end<str)
		return nullptr;
	return end;
}

//
//
// COLOR PARSING
//
//


color read_col(const char *val)
{
	char *end;
	if (val!=nullptr) {
		if (val[0]=='0' && (val[1]=='x' || val[1]=='X'))
			val += 2;
		else if (val[0]=='#')
			val++;
		if (isHexOrDec(val)) {
			unsigned long v = (unsigned long)strtoll(val, &end, 16);
			int chars = (int)(end-val);
			if (chars==1)
				return c64[v&15];
			if (chars==2 && v<=0x15) // interpret as decimal c64 color
				return c64[strtol(val, &end, 10)&15];
			if (chars<=4) { // amiga color
				u8 a = ((v>>8)&0xf0) | ((v>>12)&0x0f), b = ((v>>4)&0xf0) | ((v>>8)&0x0f);
				u8 c = ((v)&0xf0) | ((v>>4)&0x0f), d = ((v<<4)&0xf0) | ((v)&0x0f);
				if (chars==3) { color ret = { b, c, d, 0xff }; return ret; }
				color ret = { a, b, c, d };
				return ret;
			}
			if (chars<=6) { color ret = { (u8)(v>>16), (u8)(v>>8), (u8)v, 255 }; return ret; }
			color ret = { (u8)(v>>24), (u8)(v>>16), (u8)(v>>8), (u8)v }; return ret;
		} else {
			for (int c=0; c<num_colornames; c++)
				if (strncasecmp(val, colornames[c].name, strlen(colornames[c].name))==0)
					return colornames[c].col;
		}
	}
	color ret = { 0, 0, 0, 255 };
	return ret;
}


// ARGUMENTS PASSED IN TO THE PROGRAM

struct Flora {
	const char *data_file, *out_file, *background_file;
	const char *preset_str, *data_str, *extra_str, *font_name;
	const char *title_str;
	fit shape;
	sort order;
	int img_widhgt;
	double aspect;
	flower *presets;
	int num_presets;
	int legend_height;
	int title_height;
	int random_flowers;
	color background;
	color text_color;
    color name_color;
	int user_args;
	int data_found;
    bool legend_inside;


	Flora() :
	data_file(nullptr),
	out_file(nullptr),
	background_file(nullptr),
	preset_str(nullptr),
	data_str(nullptr),
	extra_str(nullptr),
	font_name(font_file),
	title_str(nullptr),
	shape(FIT_ROUND),
	order(SORT_ORIG),
	img_widhgt(8192),
	aspect(16.0/9.0),
	presets(nullptr),
	num_presets(0),
	legend_height(0),
	title_height(0),
	random_flowers(0),
	user_args(0),
	data_found(0),
    legend_inside(false)
	{
		color bg = { 255, 255, 255, 255 };
		color fg = { 0, 0, 0, 255 };
        color mg = { 0xaa, 0x55, 0xcc, 0xbd };
		background = bg;
		text_color = fg;
        name_color = mg;
	}

	int Do();
	bool Argument(const char *command, const char *arg=nullptr, bool full=false);
} flora;



//
//
// PARSE DATA RANGE
//
//

int GetRange(const char *cell, ColumnIndex type, flower &low, flower &high)
{
	while (cell && *cell==' ') cell++;
	const char *val2 = strstr(cell, "to");
	if (!val2)
		val2 = cell;
	else {
		val2 += 2;
		while (*val2 && *val2<=' ')
			val2++;
	}
	switch (type) {
		case CLN_VALUE:
			if (cell[0]=='0' && tolower(cell[1])=='x')
				low.r = sqrt(double(strtol(cell,NULL, 16)));
			else
				low.r = sqrt(strtod(cell, nullptr));	// area is value
			if (val2[0]=='0' && tolower(val2[1])=='x')
				high.r = sqrt(double(strtol(val2,NULL, 16)));
			else
				high.r = sqrt(strtod(val2, nullptr));
			low.value = cell;
			high.value = val2;
			break;

		case CLN_NAME:
			low.name = cell;
			high.name = cell;
			break;

		case CLN_CENTER:
			low.c = 0.01 * strtod(cell, nullptr);	// center is a percentage scalar of radius here
			high.c = 0.01 * strtod(val2, nullptr);
			break;

		case CLN_ANGLE:
			low.a = strtod(cell, nullptr);
			high.a = strtod(val2, nullptr);
			break;

		case CLN_PETAL:
			low.k = 0.01*strtod(cell, nullptr);	// convert percent
			high.k = 0.01*strtod(val2, nullptr);
			break;

		case CLN_COLPETAL:
			low.col_pet = read_col(cell);
			high.col_pet = read_col(val2);
			low.lin_pet = 192; // if setting color and not linear probably implying slightly linear interp
			break;

		case CLN_LINPETAL:
			low.lin_pet = (u8)(2.55 * strtod(cell, nullptr));
			high.lin_pet = (u8)(2.55 * strtod(cell, nullptr));
			break;

		case CLN_COLCTR:
			low.col_ctr = read_col(cell);
			high.col_ctr = read_col(val2);
			low.lin_ctr = 192;
			break;

		case CLN_LINCTR:
			low.lin_ctr = (u8)(2.55 * strtod(cell, nullptr));
			high.lin_ctr = (u8)(2.55 * strtod(cell, nullptr));
			break;

		case CLN_FLAT:
			low.f = strtod(cell, nullptr);
			high.f = strtod(cell, nullptr);
			break;

		case CLN_TYPE:
			low.type = atoi(cell);
			high.type = atoi(val2);
			if (low.type<3)		low.type = 3;
			if (low.type>40)	low.type = 40;
			if (high.type<3)	high.type = 3;
			if (high.type>40)	high.type = 40;
			break;

		case CLN_COUNT:
			return atoi(cell);

		default:
			break;
	}
	return 0;
}


//
//
// DATA FILE PARSING
//
//


// parse a CSV file into a 2D array of char pointers. returns nullptr if invalid or can't open, 3 mallocs.
const char** ReadCSV(const char *filename, int &columns, int &rows, const char** memory)
{
	size_t s;
	*memory = nullptr;
	if (char *CSV = ReadFile(filename, s)) {
		char *CSVStart = ((u8)CSV[0]==0xef&&(u8)CSV[1]==0xbb&&(u8)CSV[2]==0xbf)?CSV+3:CSV;
		const char *p = CSVStart;
		size_t l = s;
		bool q = false, e=true, f=true;
		int n = 0, cl=0, rw=0, z=0;	// count columns, rows and bytes for strings.
		while (l) {
			char c = *p++;
			l--;
			if (e && c>' ') e = false;
			switch (c) {
				case 0: return nullptr;
				case '"': if (f) { q=!q; f=!f; } else if (q) {
					if (*p==c && l) { z++; p++; l--; } else q = false; } break;
				case ',': if (!q) { n++; e=true; f=true; z++; } break;
				case '\r':
				case '\n': if (q) break; if (n|!f) { n++; z++;
					if (n>cl) cl=n; rw++; n=0; f=true; } break;
				default: z++; f = false; break;
			}
		}
		z++;
		if (!cl) cl = n;
		if (n) rw++;	// get room for array and strings and fill it out.
		char **ret = nullptr;
		if (cl>0 && rw>0) {
			ret = (char**)malloc(sizeof(char*) * cl * rw);
			char **o = ret;
			char *w=(char*)malloc(z); *memory=w; p = CSVStart; l = s; n = 0; *o++=w;
			while (l) {
				char c = *p++;
				l--;
				if (e && c>' ') e = false;
				switch (c) {
					case 0: return nullptr;
					case '"': if (f) { q=!q; f=!f; } else if (q) { if (*p==c && l) { *w++=c; p++; l--; } else q = false; } break;
					case ',': if (!q) { n++; e=true; *w++=0; f=true; if((o-ret)<(cl*rw)) *o++=w; } break;
					case '\r':
					case '\n': if (q) break; if (n|!f) { n++; n=0; f=true; *w++=0; if((o-ret)<(cl*rw)) *o++=w; } break;
					default: if (!e) *w++ = c; f = false; break;
				}
			}
			*w++=0;
#ifdef CSV_DEBUG
			for (int r=0; r<rw; r++) {
				for (int c=0; c<cl; c++) {
					printf("%s,", ret[r*cl+c]);
				}
				printf("\n");
			}
#endif
		}
		free(CSV);
		columns = cl;
		rows = rw;
		return (const char**)ret;
	}
	return nullptr;
}


//
//
// PARSE CONTENTS OF CSV FILES
//
//


flower* readValuesFromCSV(const char *filename, bool range, int &count, const char **mem, flower *presets=nullptr, int num_presets=0)
{
	int columns, rows;
	const char *CSV;
	flower *flowers = nullptr, *f = nullptr;
	if (const char** pCells = ReadCSV(filename, columns, rows, &CSV)) {
		if (columns && rows) { // top row will contain the names of relevant elements
			int start_row = 0, top_row = 0; // where the values start

			if (!range) {
				// Allow for command line arguments to be rows at start of CSV (may require
				//	recursion due to re-specification of the data file)
				static bool recursed = false;
				if (!recursed) {
					for (int r=0; r<rows; r++) {
						if (!flora.Argument(pCells[r*columns], pCells[r*columns+1], true)) {
							start_row = top_row = r;
							break;
						}
					}
					if (flora.data_file!=filename && strcasecmp(flora.data_file, filename)!=0) {
						recursed = true;
						flora.extra_str = CSV;
						flower *ret = readValuesFromCSV(flora.data_file, false, count, mem);
						recursed = false;
						return ret;
					}
				}
			}

			int indices[CLN_ENTRIES]; for (int i=0; i<CLN_ENTRIES; i++) indices[i] = -1;
			for (int c=0; c<columns; c++) if (pCells[c]) for (int n=0; n<CLN_ENTRIES; n++) {
				if (strcasecmp(ColumnName[n], (const char*)pCells[c+start_row*columns])==0) {
					top_row=start_row+1; indices[n] = c; if (!range) flora.data_found |= 1<<n; break;
				}
			} // if no value column was found, make a guess
			if (!range && indices[CLN_VALUE]<0 && indices[CLN_LENGTH]>=0) {
				indices[CLN_VALUE] = indices[CLN_LENGTH];
				indices[CLN_LENGTH] = -1;
				flora.data_found = (flora.data_found | (1<<CLN_VALUE)) & ~(1<<CLN_LENGTH);
			} else if (!range && indices[CLN_VALUE]<0) {
				int bestRowNums = -1;
				for (int c=0; c<columns; c++) if (!isOneOf(c, indices, CLN_ENTRIES)) {
					int numNums = 0;
					for (int r=top_row; r<rows; r++) numNums += isNumeric(pCells[r*columns+c]);
					if (numNums>bestRowNums) {
						bestRowNums = numNums; indices[CLN_VALUE] = c; flora.data_found |= 1<<CLN_VALUE;
					}
				}
			} // if no name columns was found, make a guess
			if (!range && indices[CLN_NAME]<0) {
				int bestText = -1, bestTextCol = -1;
				for (int c=0; c<columns; c++) if (!isOneOf(0, indices, CLN_ENTRIES)) {
					int score = columnTextScore(pCells, c, columns, rows);
					if (score>bestText) { bestText=score; bestTextCol = c; };
				}
				indices[CLN_NAME] = bestTextCol; flora.data_found |= bestTextCol>=0 ? (1U<<CLN_NAME) : 0;
			}
			if (notOneOf(-1, indices, CLN_ENTRIES)) { // any meaningful columns ?
				int flowersToCreate = 0; // count meaningful rows
				for (int r=top_row; r<rows; r++)
					flowersToCreate += rowIsMeaningful(pCells+r*columns, columns, indices[CLN_VALUE], indices[CLN_COUNT]);
				if (flowersToCreate) {
					flowers = f = (flower*)malloc(sizeof(flower) * flowersToCreate * (range ? 2:1));
					int n = 0;
					int *preset_selected = (int*)malloc(sizeof(int) * flowersToCreate);
					for (int r=top_row; r<rows; r++) {
						if (rowIsMeaningful(pCells+r*columns, columns, indices[CLN_VALUE], indices[CLN_COUNT])) {
							int preset = n&&num_presets?(preset_selected[n-1]+1)%num_presets : 0;
							const char **currRow = pCells+r*columns;
							if (indices[CLN_NAME]>=0 && currRow[indices[CLN_NAME]] && *currRow[indices[CLN_NAME]]) {
								for (int c=0; c<n; c++) {
									if (flowers[c].name && strcasecmp(flowers[c].name, currRow[indices[CLN_NAME]])==0) {
										preset = preset_selected[c];
										break;
									}
								}
							}
							preset_selected[n] = preset;
							flower l = presets?presets[preset*2] : default_low;
							flower h = presets?presets[preset*2+1] : default_high; // read out range for this index
							int copies = 0;
							for (int i=0; i<CLN_ENTRIES; i++) if (indices[i]>=0) {
								int v = GetRange(currRow[indices[i]], (ColumnIndex)i, l, h);
								if (v && i==CLN_COUNT) copies = v;
							}
							if (l.r>0.00001) { // don't add if radius tiny
								for (int cpy=0; cpy<copies; cpy++) {
									if (range) { *f++ = l; *f++ = h; }
									else { initflower(f, l, h); f++; }
									n++; if (n==flowersToCreate) break;
								}
							}
						}
					}
					count = n;
				}
			}
		}
		free((void*)pCells);
	}
	*mem = CSV;
	return flowers;
}

color mixColor(color a, color b, u8 m)
{
	int im = (int)m;
	color ret = {
		(u8)(((im * (int(a.r)-int(b.r)) + 127) / 255) + (int)b.r),
		(u8)(((im * (int(a.g)-int(b.g)) + 127) / 255) + (int)b.g),
		(u8)(((im * (int(a.b)-int(b.b)) + 127) / 255) + (int)b.b),
		(u8)(((im * (int(a.a)-int(b.a)) + 127) / 255) + (int)b.a)};
	return ret;
}

// If flowers have the same name they are duplictes so make them look similar
bool CombineSharedNames(flower *flowers, int count)
{
	bool sets = false;
	for (int n=1; n<count; n++) {
		for (int c=0; c<n; c++) {
			if (flowers[n].name && flowers[c].name && strcasecmp(flowers[n].name, flowers[c].name)==0) {
				sets = true;
				flowers[n].col_ctr = mixColor(flowers[n].col_ctr, flowers[c].col_ctr, 16);
				flowers[n].col_pet = mixColor(flowers[n].col_pet, flowers[c].col_pet, 16);
				flowers[n].c = (flowers[n].c-flowers[c].c)*0.2 + flowers[c].c;
				flowers[n].k = (flowers[n].k-flowers[c].k)*0.2 + flowers[c].k;
				flowers[n].f = (flowers[n].f-flowers[c].f)*0.2 + flowers[c].f;
				flowers[n].type = flowers[c].type;
			}
		}
	}
	return sets;
}

int byIndex(const char *value, const char **aList, int nList, int default_value)
{
	for (int i=0; i<nList; i++) {
		if (strncasecmp(value, aList[i], strlen(aList[i]))==0)
			return i;
	}
	return default_value;
}


bool Flora::Argument(const char *command, const char *arg, bool full)
{
	// find the argument with the longest matching prefix
	arguments best = A_COUNT;
	int mostSame = 0;
	for (int a=A_ASPECT; a<A_COUNT; a++) {
		int same = 0; const char *f=command; const char *s=cmd_args[a];
		while (*f!='=' && tolower(*f++)==tolower(*s++)) same++;
		if (same>mostSame) {
			best = (arguments)a;
			mostSame = same;
		}
	}

	// double check full name if required (arguments read from csv file)
	if (full && best<A_COUNT && strncasecmp(command, cmd_args[best], strlen(cmd_args[best]))!=0)
		return false;

	if (!arg) {
		while (*command && *command++!='=');
		arg = command;
	}

	switch (best) {
		case A_ASPECT:
			aspect = strtod(arg, nullptr);
			printf("Aspect=%f\n", aspect);
			break;
		case A_BACKGROUND: {
			background = read_col(arg);
			background_file = arg;
			u8 r = (background.r+background.g+background.b)<(128*3) ? 255 : 0;
			color l = { r, r, r, 255 };
			if (!(user_args & (1U<<A_COLOR)))
				text_color = l;
			if (!(user_args & (1U<<A_NAME_COLOR)))
				name_color = l;
			printf("Background=%s\n", arg);
			break;
		}
		case A_DATA:
			data_file = arg;
			printf("Data=%s\n", arg);
			break;
		case A_COLOR:
			text_color = read_col(arg);
			if (!(user_args & (1U<<A_NAME_COLOR)))
				name_color = text_color;
			break;
		case A_FONT:
			font_name = arg;
			printf("Font=%s\n", font_name);
			break;
		case A_LEGEND:
            if (tolower(arg[0])=='i')
                legend_inside = true;
            else {
                legend_height = atoi(arg);
                if (const char *fs=strchr(arg,':'))
                    font_name=fs+1;
                printf("Legend=%d\n", legend_height);
            }
			break;
		case A_MAKE:
			shape = (fit)byIndex(arg, fit_name, FIT_COUNT, shape);
			printf("Make=%s\n", fit_name[shape]);
			break;
		case A_NAME_COLOR:
			name_color = read_col(arg);
			break;
		case A_ORDER:
			order = (sort)byIndex(arg, sort_name, SORT_COUNT, order);
			printf("Order=%s\n", sort_name[order]);
			break;
		case A_PRESET:
			if (!presets) {
				char pbuf[512];
				const char *pset = arg;
				if (!strlastchar(pset, '\\', '/')) {
					if (const char *datapath = strlastchar(data_file, '\\', '/')) {
						memcpy(pbuf, data_file, (datapath-data_file)+1);
						memcpy(pbuf+(datapath-data_file)+1, arg, strlen(arg)+1);
						pset = pbuf;
					}
				}
				if ((presets = readValuesFromCSV(pset, true, num_presets, &preset_str)))
					printf("Preset=%s\n", arg);
			}
			break;
		case A_RANDOM:
			random_flowers = atoi(arg);
			printf("Random=%d\n", random_flowers);
			break;
		case A_SIZE:
			img_widhgt = atoi(arg);
			printf("Size=%d\n", img_widhgt);
			break;
		case A_TITLE:
			title_height = img_widhgt / 16;
			title_str = arg;
			if (const char *c=strchr(arg, ':')) {
				title_height = atoi(arg);
				title_str = c+1;
			}
			break;
		default:
			if (!full)
				printf("Unknown parameter \"%s\"\n", command);
			return false;
	}
	user_args |= 1U<<(unsigned int)best;
	return true;
}

#define EDGE_MARGIN 8
int Flora::Do()
{
	// INSTRUCTIONS CHECK
	if (out_file && (*out_file=='?' || strcasecmp(out_file, "help")==0 || strcasecmp(out_file, "-h")==0 || strcasecmp(out_file, "h")==0)) {
		fputs(Instructions, stdout);
		return 0;
	}

	// MAKE SURE RANDOM IS UNIQUE
	srand ((unsigned int)time(NULL));

	// LOAD OR RANDOMIZE SET OF FLOWERS
	int num_flowers = 0;
	flower* flowers = nullptr;
	if (data_file)
		flowers = readValuesFromCSV(data_file, false, num_flowers, &data_str, presets, num_presets);

	if (random_flowers) {
		flower *flowers_rand = (flower*)malloc(sizeof(flower) * (random_flowers+num_flowers));
		if (flowers) {
			memcpy(flowers_rand, flowers, sizeof(flower) * num_flowers); free(flowers); flowers = nullptr;
		}
		int first_rand = num_flowers;
		num_flowers += random_flowers;
		flowers = flowers_rand;
		for (int n=first_rand; n<num_flowers; n++) {
			if (num_presets)
				initflower(flowers_rand+n, presets[(n%num_presets)*2], presets[(n%num_presets)*2+1]);
			else
				initflower(flowers_rand+n, default_low, default_high);
		}
	}
	bool sets = CombineSharedNames(flowers, num_flowers);

	// RELEASE PRESETS
	if (presets) {
		free(presets);
		presets = nullptr;
	}

	// CHECK IF THERE IS ANY WORK
	if (!flowers) {
		fputs("Nothing to do\n", stdout);
		fputs(Instructions, stdout);
		if (data_str) free((void*)data_str);
		if (preset_str) free((void*)preset_str);
		return 0;
	}

	// CHECK IMPLIED ARGUMENTS
	char out_file_buf[512], title_buf[128];
	if (!out_file) {//"flowers.png"
		if (data_file) {
			int last_dot=(int)strlen(data_file)-1;
			while (last_dot>=0 && data_file[last_dot]!='.') last_dot--;
			if (last_dot<(sizeof(out_file_buf)-5)) {
				out_file = out_file_buf;
				memcpy(out_file_buf, data_file, last_dot);
				memcpy(out_file_buf+last_dot, ".png", 5);
			}
		}
		if (!data_file)
			data_file = "flowers.png";
	}
	if (!legend_height && (data_found&((1U<<CLN_NAME)|(1U<<CLN_VALUE))) == ((1U<<CLN_NAME)|(1U<<CLN_VALUE))) {
        legend_height = legend_inside ? 0 : (img_widhgt>>6);
		if (!title_height && data_file) {
			int last_sls=(int)strlen(data_file), max_dot=last_sls;
			while (last_sls>=0 && data_file[last_sls]!='\\' && data_file[last_sls]!='/') last_sls--;
			int first_dot = last_sls+1;
			while (first_dot<max_dot && data_file[first_dot]!='.') first_dot++;
			memcpy(title_buf, data_file+last_sls+1, first_dot-last_sls-1);
			title_buf[first_dot-last_sls-1]=0;
			char *b = title_buf; while (*b) { if (*b == '_') *b = ' '; b++; }
			title_str = title_buf;
			title_height = img_widhgt>>3;
		}
	}

	if (!(user_args&(1<<A_NAME_COLOR)))
		name_color = text_color;

	// IMAGE SETUP
	unsigned int bc = *(unsigned int*)&background;
	unsigned int *bitmap = nullptr;
	int img_wid = -1;
	int img_hgt = -1;

	// LOAD BACKGROUND IMAGE
	if (background_file) {
		const char *ext = background_file + strlen(background_file)-4;
		if (strncasecmp(ext, ".jpg", 4)==0 || strncasecmp(ext, ".gif", 4)==0 ||
			strncasecmp(ext, ".png", 4)==0 || strncasecmp(ext, ".bmp", 4)==0 ||
			strncasecmp(ext, ".tga", 4)==0)
		{
			int width, height, channels;
			if (unsigned char *data = stbi_load(background_file, &width, &height, &channels, 0)) {
				color b = { 255, 255, 255, 255 };
				background = b;
				img_wid = width; img_hgt = height;
				aspect = (double)width / (double)height;
				if (channels==4)
					bitmap = (unsigned int*)data;
				else {
					bitmap = (unsigned int*)malloc(width * height * sizeof(unsigned int));
					for (int i=0; i<(width*height); i++) {
						bitmap[i] = bc;
						for (int c=0; c<channels; c++)
							((unsigned char*)(bitmap+i))[c] = data[i*channels+c];
					}
					free(data);
				}
			}
		}
	}

	// SORT
	reorder(flowers, num_flowers, order);

	// PACK
	printf("Arranging %d flowers\n", num_flowers);
	pack_flowers(flowers, num_flowers, shape, aspect);

	// FIT TO BITMAP
	double minx=0.0, maxx=0.0, miny=0.0, maxy=0.0;
	int scale_op = 0;
	while (scale_op<2) {
		minx=DBL_MAX, maxx=-DBL_MAX, miny=DBL_MAX, maxy=-DBL_MAX;
		for (flower* f=flowers; f<(flowers+num_flowers); f++) {
			if ((f->x-f->r)<minx) minx = f->x-f->r;
			if ((f->x+f->r)>maxx) maxx = f->x+f->r;
			if ((f->y-f->r)<miny) miny = f->y-f->r;
			if ((f->y+f->r)>maxy) maxy = f->y+f->r;
		}
		if (!scale_op) {
			double maxwid = img_wid<0 ? (double)img_widhgt : (double)img_wid;
			double maxhgt = img_hgt<0 ? (double)img_widhgt : (double)img_hgt;
			double sx = (maxx-minx)/double(maxwid-EDGE_MARGIN*2);
			double sy = (maxy-miny)/double(maxhgt-EDGE_MARGIN*2);
			double s = sx>sy ? 1.0/sx : 1.0/sy;
			for (flower* f=flowers; f<(flowers+num_flowers); f++) {
				f->x = (f->x-minx)*s + EDGE_MARGIN;
				f->y = (f->y-miny)*s + EDGE_MARGIN;
				f->r *= s;
			}
		}
		scale_op++;
	}

	minx -= double(EDGE_MARGIN);
	miny -= double(EDGE_MARGIN);
	maxx += double(EDGE_MARGIN);
	maxy += double(EDGE_MARGIN);

	img_wid = img_wid<0 ? int(maxx-minx) : img_wid;
	img_hgt = img_hgt<0 ? int(maxy-miny) : img_hgt;

	double cx = 0.5*((double)img_wid-(maxx-minx));
	double cy = 0.5*((double)img_hgt-(maxy-miny));

	bool hasFont = (legend_height || legend_inside || (title_height&&title_str)) && InitFont(font_name);
	int legend_columns = 0;
	int legend_lines = 0;
	int legend_count = 0;

	// check if need room for legend
	if (title_height && hasFont) {
		img_hgt += title_height;
		cy += title_height;
	}
	double legend_center = 0.0;
	if (legend_height && hasFont && !legend_inside) {
		double scale = FontSizeScale((float)legend_height);
		double lg_minx=0, lg_maxx=0, lg_miny=0, lg_maxy=0;
		for (flower* f=flowers; f<(flowers+num_flowers); f++) {
			if (f->name) {
				// determine if unique
				bool dupe = false;
				for (flower* g=flowers; g<f && !dupe; g++) {
					if (g->name && strcasecmp(g->name, f->name)==0)
						dupe = true;
				}
				if (!dupe) {
					char text[512];
					if (f->value && !sets)
						sprintf(text, "%s: %s", f->name, f->value);
					else
						sprintf(text, "%s", f->name);
					textspace space = GetTextSpace((const unsigned char*)text);
					if (lg_minx>(scale*space.minx)) lg_minx = scale*space.minx;
					if (lg_maxx<(scale*space.maxx)) lg_maxx = scale*space.maxx;
					if (lg_miny>(scale*space.miny)) lg_miny = scale*space.miny;
					if (lg_maxy<(scale*space.maxy)) lg_maxy = scale*space.maxy;
					legend_count++;
				}
			}
		}
		lg_maxx += legend_height; // room for flower :)

		if (legend_count) {
			// can ignore lg_minx
			legend_columns = 1;
			if (img_wid<(lg_maxx + legend_height*2))
				img_wid = int(lg_maxx + legend_height*2);
			else {
				legend_columns = (img_wid/int(lg_maxx + legend_height*2)) < legend_count ?
					img_wid/int(lg_maxx + legend_height*2) : legend_count;
			}
			legend_lines = (legend_count + legend_columns-1) / legend_columns;
			legend_center = 0.5 * (img_wid/legend_columns - lg_maxx - legend_height);

			if (!bitmap)
				img_hgt += legend_height * legend_lines;
		}
	}

	int img_size = img_wid * img_hgt * sizeof(unsigned int);

	if (!bitmap) {
		bitmap = (unsigned int*)malloc(img_size);
		if (bitmap) for (int i=0; i<(img_wid * img_hgt); i++)
			bitmap[i] = bc;
	}

	if (!bitmap) {
		printf("Could not allocate memory for %d x %d pixels (%d MB)\n",
			   img_wid, img_hgt, img_wid*img_hgt*4/(1024*1024));
		if (data_str) free((void*)data_str);
		if (preset_str) free((void*)preset_str);
		if (extra_str) free((void*)extra_str);
		if (flowers) free((void*)flowers);
		return 1;
	}

	printf("Image size: %.d, %d\nPainting %d flowers..\n", img_wid, img_hgt, num_flowers);
	for (flower* f=flowers; f<(flowers+num_flowers); f++) {
		drawnpetal(bitmap, img_wid, f->x + cx, f->y + cy, f->r, f->c * f->r, f->a, 0.05 / (f->k), f->f,
				  *(unsigned int*)&f->col_pet, *(unsigned int*)&f->col_ctr, f->type);
	}

	if (title_height && title_str && hasFont) {
		printf("Adding title \"%s\"\n", title_str);
		double scale = FontSizeScale((float)title_height);
		int baseline = FontBaseline();
		textspace box = GetTextSpace((unsigned const char*)title_str);
		if (scale*(box.maxx - box.minx) > (img_wid + 2 * EDGE_MARGIN)) scale = double(img_wid + 2 * EDGE_MARGIN) / double(box.maxx - box.minx);
		DrawTextAt((const unsigned char*)title_str, (float)scale, 0.5*(img_wid-scale*(box.maxx-box.minx)),
				   scale*baseline, text_color, bitmap, img_wid, img_hgt);
	}
    if (legend_inside && hasFont) {
        printf("Adding legend inside..\n");
        for (flower* f=flowers; f<(flowers+num_flowers); f++) {
            if (f->name) {
                textspace box = GetTextSpace((unsigned const char*)f->name);
                color c = name_color;
                double w = box.maxx-box.maxy, h = box.maxy-box.miny;
                double mh = 0.5*(box.minx+box.maxx), mv = 0.5*(box.miny+box.maxy);
                double scale = 2.0*f->r/sqrt(w*w+h*h);
                DrawTextAt((const unsigned char*)f->name, (float)scale, f->x+cx-scale*mh, f->y+cy-scale*mv, c, bitmap, img_wid, img_hgt);
            }
        }
    } else if (legend_height && hasFont) {
		printf("Adding legend..\n");
		double scale = FontSizeScale((float)legend_height);
		int centerHgt = FontCenterHgt();
		int n = 0;
		for (flower* f=flowers; f<(flowers+num_flowers); f++) {
			if (f->name) {
				bool dupe = false;
				for (flower* g=flowers; g<f && !dupe; g++) {
					if (g->name && strcasecmp(g->name, f->name)==0)
						dupe = true;
				}
				if (!dupe) {
					char text[512];
					if (f->value && !sets)
						sprintf(text, "%s: %s", f->name, f->value);
					else
						sprintf(text, "%s", f->name);
					color c = name_color;
					double y = img_hgt-(legend_lines-n/legend_columns) * legend_height - EDGE_MARGIN+scale*centerHgt;
					double x = (img_wid/legend_columns) * (n%legend_columns) + legend_height + legend_center;
					drawnpetal(bitmap, img_wid, x, y, 0.5*legend_height, f->c * 0.5 * legend_height, 0.0, 0.25/(f->k*f->k), f->f,
							  *(unsigned int*)&f->col_pet, *(unsigned int*)&f->col_ctr, f->type);
					DrawTextAt((const unsigned char*)text, (float)scale, x+legend_height,
							   y+scale*centerHgt, c, bitmap, img_wid, img_hgt);
					n++;
				}
			}
		}
	}

	free(flowers);
	ShutdownFont();
	if (data_str) free((void*)data_str);
	if (preset_str) free((void*)preset_str);
	if (extra_str) free((void*)extra_str);

	if (out_file) {
		printf("Saving result as \"%s\"...\n", out_file);
		size_t out_file_len = strlen(out_file);
		if (out_file_len>=4 && strcasecmp(out_file+out_file_len-4, ".png")==0)
			stbi_write_png(out_file, img_wid, img_hgt, 4, bitmap, 0);
		if (out_file_len>=4 && strcasecmp(out_file+out_file_len-4, ".tga")==0)
			stbi_write_tga(out_file, img_wid, img_hgt, 4, bitmap);
		if (out_file_len>=4 && strcasecmp(out_file+out_file_len-4, ".bmp")==0)
			stbi_write_bmp(out_file, img_wid, img_hgt, 4, bitmap);
	}

	free(bitmap);
	printf("Done!\n");
	return 0;
}


//
//
// ENTRY POINT
//
//

int main(int argc, char **argv)
{
	// PARSE COMMAND LINE ARGUMENTS
	for (int a=1; a<argc; a++) {
		if (const char *arg = strchr(argv[a], '=')) {
			if (!flora.Argument(argv[a], arg+1))
				printf("unknown property pair %s\n", argv[a]);
		} else {
			int argl = (int)strlen(argv[a]); const char *arge=argv[a]+argl;
			if (argl>3) {
				if (strcasecmp(arge-4, ".csv")==0)
					flora.data_file = argv[a];
				else if ( strcasecmp(arge-4, ".png")==0 ||
						  strcasecmp(arge-4, ".tga")==0 ||
						  strcasecmp(arge-4, ".bmp")==0)
					flora.out_file = argv[a];
			}
		}
	}

	return flora.Do();
}


