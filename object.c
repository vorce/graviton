/*
* object.c
* created on: Fri Mar  5 02:22:42 2004
* by: Joel Carlbark
*/

#include <math.h>

#include "object.h"

int idc=0;

/* sphere stuff

    This is a major bottle-neck. Optimize! */
float
obj_sphereIntersect(void *o, t_ray r)
{
    float a, b, c, d, t=0.0, t1, t2;
    float oa, od;

    t_object s=*((t_object *)o);

    a=vectorDot(r.direction, r.direction);

    b=2*( r.direction.x*(r.origin.x-s.position.x)
        + r.direction.y*(r.origin.y-s.position.y)
        + r.direction.z*(r.origin.z-s.position.z)
        );

    c=(r.origin.x-s.position.x);
    c*=c;
    c+=( (r.origin.y-s.position.y)*(r.origin.y-s.position.y) );
    c+=( (r.origin.z-s.position.z)*(r.origin.z-s.position.z) );
    c-=( (s.radius*s.radius) );

    oa=2*a;

    d=(b*b)-(2*oa*c);
    if(d<0.0)
        return 0.0;

    od=sqrt(d);

    if(d==0.0)
        t=(-b+od) / oa;
    else if(d>0.0)
    {
        t1=(-b+od) / oa;
        t2=(-b-od) / oa;
        t=t1>t2?t1:t2;  /* C gibberish :-) */
    }

    return t;
}

vector3df
obj_sphereNormal(void *o, vector3df i)
{
    vector3df v;
    t_object s=*((t_object *)o);

    v.x=(i.x-s.position.x)/s.radius;
    v.y=(i.y-s.position.y)/s.radius;
    v.z=(i.z-s.position.z)/s.radius;

    return v;
}

t_object
obj_sphereDefine(vector3df pos, float radius)
{
    t_object o;

    o.type=OBJ_SPHERE;
    o.intersectfunc=obj_sphereIntersect;
    o.normalfunc=obj_sphereNormal;
    o.position=pos;
    o.radius=radius;
    idc+=1;
    o.id=idc;
    /* o.color is not set! */

    return o;
}

float
obj_planeIntersect(void *o, t_ray r)
{
    float a, b, t;
    t_object p=*((t_object *)o);

    a=r.direction.x*p.position.x+r.direction.y*p.position.y+
        r.direction.z*p.position.z;
    if(a!=0.0)
    {
        b=-(r.origin.x*p.position.x+r.origin.y*p.position.y+
            r.origin.z*p.position.z+p.radius);
        t=b/a;
    }
    else
        t=0.0;
    return t;
}

vector3df
obj_planeNormal(void *o, vector3df i)
{
    t_object p=*((t_object *)o);
    return p.position;
}

t_object
obj_planeDefine(vector3df pos, float size)
{
    t_object o;

    o.type=OBJ_PLANE;
    o.intersectfunc=obj_planeIntersect;
    o.normalfunc=obj_planeNormal;
    o.position=pos;
    o.radius=size;
    idc+=1;
    o.id=idc;
    /* o.color is not set! */
    return o;
}

t_material
obj_materialDefine(float s, float sp, float a, float d)
{
    t_material m;

    m.shiny=s;
    m.specular=sp;
    m.ambient=a;
    m.diffuse=d;
    m.reflectivity=0.0;
    m.alpha=0.0;
    return m;
}

