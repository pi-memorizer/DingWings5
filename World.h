#ifndef WORLD_H
#define WORLD_H
#include "Revengine.h"

extern Sprite **tileset; //array of size 65536 that holds all of the tiles

//Generic world class, currently only extended by StaticWorld, but could be extended for more dynanmic worlds
class World
{
public:
	List<Entity*> entities; //list of entities in the world
	void virtual run() = 0; //run every frame, good for testing if players step on things
	void virtual draw(Player *p) = 0;
	void virtual enter(Player *p) = 0;
	bool virtual interact(Player *p, int x, int y) = 0;
	bool virtual collides(int x, int y, int width, int height) = 0;
	int virtual getLower(int x, int y) = 0;//lower map layer
	int virtual getUpper(int x, int y) = 0;//upper map layer
	int virtual getEffect(int x, int y) = 0; //effect layer
	int virtual getCollision(int x, int y) = 0; //collision layer
	bool virtual setLower(int v, int x, int y) = 0;//lower map layer
	bool virtual setUpper(int v, int x, int y) = 0;//upper map layer
	bool virtual setEffect(int v, int x, int y) = 0; //effect layer
	bool virtual setCollision(int v, int x, int y) = 0; //collision layer
	void addEntity(Entity *e);
};

//class that holds the data for individual spaces in StaticWorld
struct StaticBlock
{
	unsigned short lower, upper;
	unsigned char effect, collision;

	StaticBlock(int _lower, int _upper, int _effect, int _collision)
	{
		lower = (unsigned short)_lower;
		upper = (unsigned short)_upper;
		effect = (unsigned char)_effect;
		collision = (unsigned char)_collision;
	}
	StaticBlock() {}
};

//Worlds that do not change (much) and can be loaded from a file
class StaticWorld : public World
{
private:
	void(*_run)();
	void(*_draw)(Player *p);
	bool(*_interact)(Player*,int,int);
	void(*_enter)(Player *p);
	int width, height, xOffset, yOffset;
	StaticBlock *map = nullptr;
	bool subCollision(int x, int y, int px, int py, int pwidth, int pheight);
public:
	StaticWorld(string filename, void (*enter)(Player *p), void (*run)(), void (*draw)(Player*),bool(*interact)(Player*,int,int));
	
	virtual void run();
	virtual void draw(Player *p);
	void enter(Player *p);
	bool interact(Player *p, int x, int y);
	bool virtual collides(int x, int y, int width, int height);
	int virtual getLower(int x, int y);
	int virtual getUpper(int x, int y);
	int virtual getEffect(int x, int y);
	int virtual getCollision(int x, int y);
	bool virtual setLower(int v, int x, int y);//lower map layer
	bool virtual setUpper(int v, int x, int y);//upper map layer
	bool virtual setEffect(int v, int x, int y); //effect layer
	bool virtual setCollision(int v, int x, int y); //collision layer
};

#endif