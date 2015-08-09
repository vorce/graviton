#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159
#endif

#include "SDL.h"
#include "SDL_image.h"

#include "graviton.h"
#include "camera.h"
#include "object.h"
#include "light.h"
#include "font.h"

#define N_OBJS 4
#define N_LIGHTS 1

#define AA_THRESHOLD 125
/* #define ANTI_ALIAS 1 */

const int XRESH=(XRES/2);
const int YRESH=(YRES/2);

extern SDL_Color ray_trace(t_ray, t_object *, int, t_light *, int, int);

/* scene description format..ini style..suckage majoris. */
#if 0
typedef struct
{
    unsigned char on;
    SDL_Color c;
    float density;
} t_fog;

typedef struct
{
    char *name;

    t_fog fog;

    int nos;
    t_object *os;

    int nls;
    t_light *ls;
} t_scene;

t_scene
parse_scene(char *file)
{
    t_scene s;
    dictionary *ini;
    int i, j;
    int np, ns;
    char *tmp;

    ini=iniparser_load(file);
    if(ini==NULL)
    {
        printf("Couldn't parse scene file: %s\n", file);
        s.nos=s.nls=-1;
        return s;
    }

    s.name=iniparser_getstr(ini, "global:name");
    if(!s.name)
        s.name="Untitled";
    printf("Name of scene: %s\n", s.name);

    s.fog.on=iniparser_getboolean(ini, "global/fog:enabled", -1);
    printf("Fog: %d\n", s.fog.on);
    s.fog.c.red=iniparser_getint(ini, "global/fog:red", -1);
    s.fog.c.green=iniparser_getint(ini, "global/fog:green", -1);
    s.fog.c.blue=iniparser_getint(ini, "global/fog:blue", -1);
    printf("\tFog color: %d %d %d\n", s.fog.c.red, s.fog.c.green, s.fog.c.blue);
    s.fog.density=iniparser_getdouble(ini, "global/fog:density", 1.0);
    printf("\tFog density: %g\n", s.fog.density);

    ns=iniparser_getint(ini, "objects:nspheres", 0);
    np=iniparser_getint(ini, "objects:nplanes", 0);
    if((ns+np)>0)
    {
        s.nos=(ns+np);
        s.os=(t_object *)malloc(sizeof(t_object)*s.nos);

        for(i=0;i<ns;i++)
        {
            tmp=(char *)malloc(sizeof(char)*strlen("objects/sphere1:posx"));
        /*
            loop thru nos:
                make string: sphere<i>
                ret=iniparser_getdouble(ini, "objects/string:posx", -9999.9);
                if(ret==-9999.9)
                {
                    make string: plane<i> */
    }
#endif  /* 0 */

long
getTime(void)
{
    return SDL_GetTicks();
}

long
getTimeDif(long time)
{
    return getTime()-time;
}

Uint32
getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp=surface->format->BytesPerPixel;
    Uint8 *p=(Uint8 *)surface->pixels+y*surface->pitch+x*bpp;
    switch(bpp)
    {
        case 1:
            return *p;
        case 2:
            return *(Uint16 *)p;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32 *)p;

        default:
            return 0;
    }
}

void
filter_blur(SDL_Surface *l, int x1, int y1, int x2, int y2)
{
    SDL_Color cn, cs, ce, cw;
    Uint16 *p=NULL;
    int x=0, y=0;

    for(y=y1;y<y2;y++)
    {
        p=(Uint16 *)l->pixels+y*l->pitch/2+x;
        for(x=x1;x<x2;x++)
        {
            SDL_GetRGB(getpixel(l, x, y-1), l->format, &cn.r, &cn.g, &cn.b);
            SDL_GetRGB(getpixel(l, x, y+1), l->format, &cs.r, &cs.g, &cs.b);
            SDL_GetRGB(getpixel(l, x+1, y), l->format, &ce.r, &ce.g, &ce.b);
            SDL_GetRGB(getpixel(l, x-1, y), l->format, &cw.r, &cw.g, &cw.b);
            *p=SDL_MapRGB(l->format, (cs.r+cn.r+ce.r+cw.r)>>2, (cs.g+cn.g+ce.g+cw.g)>>2, (cs.b+cn.b+ce.b+cw.b)>>2);
            p++;
        }
    }
}

void
checkAliasing(SDL_Surface *l, int x, int y, SDL_Color o)
{
    SDL_Color c[6];
    int s=0, i;
    int r=0, g=0, b=0;
    unsigned char anti=0;
    Uint16 *p=NULL;

    p=(Uint16 *)l->pixels+y*l->pitch/2+x;
    if(x==l->w || x>0)
    {
        SDL_GetRGB(getpixel(l, x-1, y), l->format, &c[s].r, &c[s].g, &c[s].b);
        s++;
    }
    if(x==0 || x<l->w)
    {
        SDL_GetRGB(getpixel(l, x+1, y), l->format, &c[s].r, &c[s].g, &c[s].b);
        s++;
    }
    if(y==l->h || y>0)
    {
        SDL_GetRGB(getpixel(l, x, y-1), l->format, &c[s].r, &c[s].g, &c[s].b);
        s++;
    }
    if(y==0 || y<l->h)
    {
        SDL_GetRGB(getpixel(l, x, y+1), l->format, &c[s].r, &c[s].g, &c[s].b);
        s++;
    }
    if(y<l->h && x<l->w)
    {
        SDL_GetRGB(getpixel(l, x+1, y+1), l->format, &c[s].r, &c[s].g, &c[s].b);
        s++;
    }
    if(y>0 && x>0)
    {
        SDL_GetRGB(getpixel(l, x-1, y-1), l->format, &c[s].r, &c[s].g, &c[s].b);
        s++;
    }

    for(i=0;i<s;i++)
    {
        if( ((o.r-c[i].r)+(o.g-c[i].g)+(o.b-c[i].b)) >= AA_THRESHOLD)
            anti=1;
    }

    if(anti==1)
    {
        for(i=0;i<s;i++)
        {
            r+=c[i].r;
            g+=c[i].g;
            b+=c[i].b;
        }
        r/=s; g/=s; b/=s;
        *p=SDL_MapRGB(l->format, r, g, b);
    }
}

int
main(int argc, char *argv[])
{
    SDL_Surface *screen, *font;
    SDL_Event event;
    char quit=0;
    int x=0, y;
    t_camera cam;
    t_ray ray;
    t_object objs[N_OBJS];
    t_light lights[N_LIGHTS];
    int perspective=256;
    SDL_Color color;
    Uint16 *p=NULL;
    long time, tim2;
    int frame=0;

    printf("%s %s\n", APPNAME, VERSION);

    font=font_init("data/font.png");

    cam.position=vectorCreate(0, 0, -(perspective));

    SDL_Init(SDL_INIT_VIDEO);
    screen=SDL_SetVideoMode(XRES, YRES, BPP, SDL_HWSURFACE);
    SDL_WM_SetCaption(APPNAME, NULL);

    objs[0]=obj_sphereDefine(vectorCreate(-85, -45, 100), 60);
    objs[0].color.r=14; objs[0].color.g=120; objs[0].color.b=160;
    objs[0].material=obj_materialDefine(1.0, 1.5, 0.9, 2.0);
    objs[0].material.reflectivity=0.5;
    objs[0].material.alpha=0.5;

    objs[1]=obj_sphereDefine(vectorCreate(85, -45, 100), 60);
    objs[1].color.r=160; objs[1].color.g=60; objs[1].color.b=14;
    objs[1].material=obj_materialDefine(1.5, 1.5, 1.7, 3.5);
    objs[1].material.reflectivity=0.5;
    objs[1].material.alpha=0.5;

    objs[2]=obj_planeDefine(vectorCreate(0, 1, 0), -200);
    objs[2].color.r=240; objs[2].color.g=240; objs[2].color.b=140;
    objs[2].material=obj_materialDefine(0.5, 1.0, 1.0, 2.0);
    objs[2].material.reflectivity=0.0;
    objs[2].material.alpha=1.0;

    objs[3]=obj_sphereDefine(vectorCreate(0, 30, 100), 45);
    objs[3].color.r=14; objs[3].color.g=120; objs[3].color.b=20;
    objs[3].material=obj_materialDefine(1.2, 0.4, 1.0, 2.0);
    objs[3].material.reflectivity=0.8;
    objs[3].material.alpha=0.5;

    lights[0]=light_defineLight(vectorCreate(0.0, 60.0, 150.0), 1.5);

    #ifdef DEBUG
        SDL_ShowCursor(SDL_DISABLE);
    #endif  /* DEBUG */

    time=getTime();
    while(quit==0)
    {
        SDL_FillRect(screen, 0, 0);
        SDL_LockSurface(screen);

        for(y=-(YRESH);y<YRESH;y++)
        {
            p=(Uint16 *)screen->pixels+(y+YRESH)*screen->pitch/2+(x+XRESH);
            for(x=-(XRESH);x<XRESH;x++)
            {
                /* here's where we would do oversampling i suppose */

                ray.origin=vectorCreate(cam.position.x, cam.position.y, cam.position.z);
                ray.direction=vectorCreate(x, y, perspective);
                ray.direction=vectorNormalize(ray.direction);

                color=ray_trace(ray, objs, N_OBJS, lights, N_LIGHTS, 1);
                if(color.r!=0 && color.g!=0 && color.b!=0)
                    *p=SDL_MapRGB(screen->format, color.r, color.g, color.b);

                #ifdef ANTI_ALIAS   /* this blows */
                    checkAliasing(screen, x+XRESH, y+YRESH, color);
                #endif

                p++;
            }
        }
        /* filter_blur(screen, 0, 0, XRES, YRES); */
        SDL_UnlockSurface(screen);

        #ifdef DEBUG
            font_print(screen, font, 1, 1, "%s %s\n", APPNAME, VERSION);
            font_print(screen, font, 1, 13, "light %.1f %.1f %.1f", lights[0].position.x,
                lights[0].position.y, lights[0].position.z);
            /* font_print(screen, font, 1, 27, "cam   %.1f %.1f %.1f", cam.position.x, cam.position.y,
                cam.position.z); */

            p=(Uint16 *)screen->pixels+((int)(lights[0].position.y+YRESH))*screen->pitch/2+((int)(lights[0].position.x+XRESH));
            *p=SDL_MapRGB(screen->format, 0, 255, 0);

            if((tim2=getTimeDif(time)/1000)!=0)
                font_print(screen, font, 1, 27, "fps %d", (int)(frame/tim2));
        #endif  /* DEBUG */


        SDL_Flip(screen);
        frame++;

        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT)
                quit=1;
            else if(event.type==SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit=1;
                    break;

                    case SDLK_F1:
                        SDL_WM_ToggleFullScreen(screen);
                    break;

                    case SDLK_UP:
                        // lights[0].position.y-=2.0;
                    break;

                    case SDLK_DOWN:
                        // lights[0].position.y+=2.0;
                    break;

                    case SDLK_RIGHT:
                        // lights[0].position.x+=2.0;
                    break;

                    case SDLK_LEFT:
                        // lights[0].position.x-=2.0;
                    break;

                    case SDLK_HOME:
                        // lights[0].position.z-=2.0;
                    break;

                    case SDLK_END:
                        // lights[0].position.z+=2.0;
                    break;

                    case SDLK_SPACE:
                    break;

                    default:
                    break;
                }
            }
            else if(event.type==SDL_MOUSEMOTION)
            {
                lights[0].position.x=event.motion.x-XRESH;
                lights[0].position.y=event.motion.y-YRESH;
            }
            else if(event.type==SDL_MOUSEBUTTONDOWN)
            {
                switch(event.button.button)
                {
                    case 4:
                        lights[0].position.z+=4.0;
                    break;

                    case 5:
                        lights[0].position.z-=4.0;
                    break;

                    default:
                    break;
                }
            }
        }
    }
    SDL_FreeSurface(font);
    SDL_Quit();

    if((time=getTimeDif(time)/1000)!=0)
        printf("FPS: %d\n", (int)(frame/time));

    return 0;
}

