#ifndef SHADER_H
#define SHADER_H
#include "Collections.h"

class Player;
class Light
{
public:
	float x, y, radius, r, g, b;
	Light(float x, float y, float radius, float r = 1.0, float g = 1.0, float b = 1.0);
	Light();
};

void postProcess(Player *p);
void sDefault();
void sInvert();
void sLighting(Player *p, float ambientR, float ambientG, float ambientB, List<Light>& lights);
void sGrey();

#endif