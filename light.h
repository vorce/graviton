/* light.h created on: Fri Mar  5 23:21:30 2004 */

#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

typedef struct
{
    unsigned char r, g, b;
    float brightness;
    unsigned char on, shadows;
    vector3df position;
} t_light;

t_light light_defineLight(vector3df, float);

#endif /* LIGHT_H */

