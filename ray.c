/*
* ray.c
* created on: Fri Mar  5 00:35:39 2004
* by: Joel Carlbark
*/

#include <stdlib.h>
#include <math.h>

#include "SDL.h"
#include "object.h"
#include "light.h"

#define MAX_DEPTH 5

double
ray_shade(vector3df in, vector3df normal, vector3df view, t_light *l, int ls, t_object o)
{
    int i;
    double light=0.0;

    for(i=0;i<ls;i++)
    {
        if(l[i].on)
        {
            float angle;
            vector3df tmp=vectorCreate(l[i].position.x-in.x, l[i].position.y-in.y,
                -(l[i].position.z-in.z));   /* I wonder why i've to reverse the z-axis.. */

            /* XXX handle light fall off ? */
            tmp=vectorNormalize(tmp);

            angle=vectorDot(normal, tmp);
            if(angle>0.0)
            {
                vector3df reflection;
                float refangle;
                float powt;
                float tangle=2*angle;

                light+=o.material.diffuse*angle*l[i].brightness;
                reflection=vectorCreate(tmp.x-(normal.x*tangle),
                            tmp.y-(normal.y*tangle), tmp.z-(normal.z*tangle));

                refangle=vectorDot(reflection, view);
                if(refangle<0.0)
                    powt=0.0;
                else
                    powt=(float)(pow(refangle, o.material.shiny));

                light+=o.material.specular*powt*l[i].brightness;
            }
            light+=o.material.ambient;
        }
    }
    return light;
}

#if 0   /* non recursive simple ray trace function */
SDL_Color
ray_trace(t_ray r, t_object *objs, int nobjs, t_light *l, int ls, int depth)
{
    float t=0.0;
    int i;
    float light=0.0;
    SDL_Color color;
    vector3df out, normal;

    color.r=0; color.g=0; color.b=0;
    for(i=0;i<nobjs;i++)
    {
        t=objs[i].intersectfunc(&objs[i], r);
        if(t>0.0)
        {
            out=vectorCreate((r.origin.x+r.direction.x*t),
                    (r.origin.y+r.direction.y*t),
                    (r.origin.z+r.direction.z*t)
                );

            normal=objs[i].normalfunc(&objs[i], out);
            color=objs[i].color;

            /* calculate light */
            light=ray_shade(out, normal, r.direction, l, ls, objs[i]);
            if(light!=0.0)
            {
                int tr, tg, tb;
                tr=objs[i].color.r*light;
                tg=objs[i].color.g*light;
                tb=objs[i].color.b*light;

                if(tr>255) tr=255;
                if(tr<0) tr=0;
                if(tg>255) tg=255;
                if(tg<0) tg=0;
                if(tb>255) tb=255;
                if(tb<0) tb=0;
                color.r=tr;
                color.g=tg;
                color.b=tb;
            }

            return color;
        }
    }
    return color;
}
#endif  /* 0 */


SDL_Color
ray_trace(t_ray r, t_object *objs, int nobjs, t_light *l, int ls, int depth)
{
    float t=0.0, od=99999999.9;
    int i, io=-1;
    int tr=0, tg=0, tb=0;
    double light=0.0;
    double shadval=1.0;
    SDL_Color color;
    vector3df out, normal;

    /* fog variables */
    SDL_Color fc;
    float fogd=0.002;
    double fog=0.0;
    fc.r=fc.g=fc.b=180;

    color.r=color.g=color.b=0;

    if(depth>MAX_DEPTH)
        return color;

    for(i=0;i<nobjs;i++)
    {
        t=objs[i].intersectfunc(&objs[i], r);
        if((t>0.0) && (t<od))
        {
            io=i;
            od=t;
        }
    }
    if(io==-1)
        return color;

    out=vectorCreate((r.origin.x+r.direction.x*od),
        (r.origin.y+r.direction.y*od),
        (r.origin.z+r.direction.z*od));

    normal=objs[io].normalfunc(&objs[io], out);
    color=objs[io].color;

    fog=exp(-fogd*-(r.origin.z-out.z));
    for(i=0;i<ls;i++)
    {
        /* shadows */
        vector3df lightd;
        t_ray shadow;
        float st=0.0;
        int j;

        lightd=vectorCreate((l[i].position.x-out.x), (l[i].position.y-out.y),
                (l[i].position.z)-out.z);
        lightd=vectorNormalize(lightd);
        shadow.origin=out;
        shadow.direction=lightd;
        for(j=0;j<nobjs;j++)
        {
            if(objs[j].id!=objs[io].id)
            {
                st=objs[j].intersectfunc(&objs[j], shadow);
                if(st!=0.0)
                    shadval=objs[j].material.alpha;
            }
        }
        if(shadval<0.01)
            break;

        light=ray_shade(out, normal, r.direction, l, ls, objs[io]);
        if(light>0.0)
        {
            tr=color.r*light;
            tg=color.g*light;
            tb=color.b*light;

            tr*=shadval;
            tg*=shadval;
            tb*=shadval;
        }
    }

    /* reflection (XXX: possible errors here) */
    if(objs[io].material.reflectivity>0.0)
    {
        t_ray reflection;
        SDL_Color rc;
        vector3df n, d;
        float dot, ods=0.0;

        rc.r=rc.g=rc.b=0;
        n=normal;
        d=vectorCreate(-(r.direction.x), -(r.direction.y), -(r.direction.z));
        //d=vectorNormalize(d);

        dot=vectorDot(d, n);
        ods=od*0.00001f;
        dot=-dot;
        if(dot>0.0)
        {
            dot*=2;

            reflection.direction.x=n.x*dot+d.x;
            reflection.direction.y=n.y*dot+d.y;
            reflection.direction.z=n.z*dot+d.z;

            reflection.origin.x=out.x+reflection.direction.x*ods;
            reflection.origin.y=out.y+reflection.direction.y*ods;
            reflection.origin.z=out.z+reflection.direction.z*ods;

            rc=ray_trace(reflection, objs, nobjs, l, ls, depth+1);
            if(rc.r!=0 && rc.g!=0  && rc.b!=0)
            {
                tr=(rc.r*objs[io].material.reflectivity)+color.r;
                tg=(rc.g*objs[io].material.reflectivity)+color.g;
                tb=(rc.b*objs[io].material.reflectivity)+color.b;
            }
        }
    }

    /* fog
    tr=((fog*tr)+((1-fog)*fc.r));
    tg=((fog*tg)+((1-fog)*fc.g));
    tb=((fog*tb)+((1-fog)*fc.b));*/

    /* sanity check colors */
    if(tr>255) tr=255;
    if(tr<0) tr=0;
    if(tg>255) tg=255;
    if(tg<0) tg=0;
    if(tb>255) tb=255;
    if(tb<0) tb=0;

    color.r=tr;
    color.g=tg;
    color.b=tb;

    return color;
}

