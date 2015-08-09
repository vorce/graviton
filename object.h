/* object.h created on: Fri Mar  5 01:55:25 2004 */

#ifndef OBJECT_H
#define OBJECT_H

#include "SDL.h"

#include "ray.h"

#define OBJ_SPHERE 0
#define OBJ_CUBE 1
#define OBJ_BOX 2
#define OBJ_CYLINDER 3
#define OBJ_PLANE 4
/* and so on */

typedef struct
{
    float shiny;
    float ambient;
    float specular;
    float diffuse;
    float reflectivity;
    float alpha;
} t_material;

typedef float (*intersectFunc)(void *, t_ray);
typedef vector3df (*normalFunc)(void *, vector3df);
typedef struct
{
    SDL_Color color;
    int type;
    int id;
    intersectFunc intersectfunc;    /* for interesction */
    normalFunc normalfunc;  /* to calculate normals, for light */

    vector3df position;
    float radius;

    t_material material;
} t_object;

t_object obj_sphereDefine(vector3df, float);
t_object obj_planeDefine(vector3df, float);
t_material obj_materialDefine(float, float, float, float);

#endif /* OBJECT_H */

