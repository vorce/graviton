#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
    float x, y, z;
} vector3df;

vector3df vectorCreate(float, float, float);
vector3df vectorAdd(vector3df, vector3df);
vector3df vectorSub(vector3df, vector3df);
float vectorDot(vector3df, vector3df);
vector3df vectorCross(vector3df, vector3df);
vector3df vectorNormalize(vector3df);
float vectorMagnitude(vector3df);
float vectorDistance(vector3df, vector3df);
void vectorPrint(vector3df);

#endif  /* VECTOR_H */

