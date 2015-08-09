/*
* light.c
* created on: Fri Mar  5 23:47:50 2004
* by: Joel Carlbark
*/

#include "light.h"

t_light
light_defineLight(vector3df pos, float brightness)
{
    t_light l;
    l.position=pos;
    l.brightness=brightness;
    l.on=1;
    l.shadows=1;
    l.r=l.g=l.b=128;
    return l;
}

