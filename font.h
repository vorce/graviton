/* font.h created on: Tue Mar 23 20:16:11 2004 */

#ifndef FONT_H
#define FONT_H

#include "SDL.h"
#include "SDL_image.h"

SDL_Surface * font_init(char *);
void font_print(SDL_Surface *, SDL_Surface *, int, int, char *, ...);

#endif /* FONT_H */

