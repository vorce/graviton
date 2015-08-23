/*
* font.c
* created on: Tue Mar 23 20:16:05 2004
* by: Joel Carlbark
*
* Please consider this a horrible, mad hack.
* I wanted some easy debugging and text output, and i found
* font.png laying around randomly. This is the result *shrug*.
*/

#include <ctype.h>
#include <stdarg.h>

#include "SDL.h"
#include "SDL_image.h"


#define LETTER_W 8
#define LETTER_H 5
#define LETTER_SPACE 1
#define MAGIC_ASCII_NUMBER 97
#define FONT_NUMBERS 42

#define BUFLEN 4096

SDL_Surface *
font_init(char *filename)
{
    SDL_Surface *img;

    img=IMG_Load(filename);
    return img;
}

void
font_print(SDL_Surface *to, SDL_Surface *font, int x, int y, char *format, ...)
{
    va_list arglist;
    char buffer[BUFLEN];
    char *c;
    int ci=0;
    SDL_Rect l, t;
    l.w=LETTER_W;
    l.h=LETTER_H;
    l.x=0; l.y=1;
    t.w=l.w;
    t.h=l.h;
    t.x=x;
    t.y=y;
    va_start(arglist, format);
    vsnprintf(buffer, BUFLEN, format, arglist);
    va_end(arglist);
    c=buffer;

    // SDL_SetColorKey(to, SDL_SRCCOLORKEY, SDL_MapRGB(to->format, 0, 0, 0));
    while(*c!='\0')
    {
        ci=(int)*c;
        ci=tolower(ci);
        if(ci==' ')
            t.x+=LETTER_W;
        if(ci=='\n')
        {
            t.y+=(LETTER_H+1);
            t.x=x;
        }

        /* ok. I've to do this crap because of the font file itself,
            i pixeled it. and well..it's bad. */
        if(ci=='.')
        {
            l.x=33*(LETTER_W+LETTER_SPACE);
            SDL_BlitSurface(font, &l, to, &t);
            t.x+=(LETTER_W+1);
        }
        if(ci=='-')
        {
            l.x=31*(LETTER_W+LETTER_SPACE);
            SDL_BlitSurface(font, &l, to, &t);
            t.x+=(LETTER_W+1);
        }
        else if(ci<=57 && ci>=48)   /* numbers */
        {
            ci-=48;
            l.x=ci*(LETTER_W+LETTER_SPACE);
            l.x+=(FONT_NUMBERS*(LETTER_W+LETTER_SPACE));
            SDL_BlitSurface(font, &l, to, &t);
            t.x+=(LETTER_W+1);
        }
        else if(ci>=MAGIC_ASCII_NUMBER) /* letterz n shit, yo */
        {
            ci-=MAGIC_ASCII_NUMBER;
            l.x=ci*(LETTER_W+LETTER_SPACE);
            SDL_BlitSurface(font, &l, to, &t);
            t.x+=(LETTER_W+1);
        }
        c++;
    }
}

