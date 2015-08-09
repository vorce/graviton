#include <stdio.h>
#include <math.h>

#include "vector.h"

vector3df
vectorCreate(float x, float y, float z)
{
    vector3df new;

    new.x=x;
    new.y=y;
    new.z=z;

    return new;
}

vector3df
vectorAdd(vector3df a, vector3df b)
{
    vector3df c;

    c.x=a.x+b.x;
    c.y=a.y+b.y;
    c.z=a.z+b.z;

    return c;
}

vector3df
vectorSub(vector3df a, vector3df b)
{
    vector3df c;

    c.x=a.x-b.x;
    c.y=a.y-b.y;
    c.z=a.z-b.z;

    return c;
}

float
vectorDot(vector3df a, vector3df b)
{
    return(a.x*b.x + a.y*b.y + a.z*b.z);
}

vector3df
vectorCross(vector3df a, vector3df b)
{
    vector3df c;

    c.x=(a.y*b.z)-(a.z*b.y);
    c.y=(a.z*b.x)-(a.x*b.z);
    c.z=(a.x*b.y)-(a.y*b.x);

    return c;
}

float
vectorMagnitude(vector3df a)
{
    return(sqrt(a.x*a.x + a.y*a.y + a.z*a.z));
}

vector3df
vectorNormalize(vector3df a)
{
    vector3df normed;
    float len=vectorMagnitude(a);

    normed.x=a.x/len;
    normed.y=a.y/len;
    normed.z=a.z/len;

    return normed;
}

void
vectorPrint(vector3df a)
{
    printf("vector: x=%f, y=%f, z=%f\n", a.x, a.y, a.z);
}


