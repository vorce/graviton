/* ray.h created on: Fri Mar  5 00:11:35 2004 */

#ifndef RAY_H
#define RAY_H

#include "SDL.h"
#include "vector.h"

typedef struct
{
    vector3df origin;
    vector3df direction;
} t_ray;

#endif /* RAY_H */

