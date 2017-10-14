#ifndef ENTITY_H
#define ENTITY_H
#include "Revengine.h"

//Class for any entities that appear on the screen
//If your entity does not move and can be displayed on a block instead of a sprite,
//please using an interaction script instead (World.h)

class Entity
{
public:
	bool isAlive = true; //entity will be removed once this is false
	int x, y, width = TILE_SIZE, height = TILE_SIZE; //coordinates and size in pixel coordinates

	//Creates an entity in a given world at the specified coordinates with a certain size and sprite
	//function pointers pass in are used for run and interact methods, for convenience
	//if you don't want to use function pointers, this class will have to be extended
	Entity(World *world, int x, int y);
	virtual void run();
	virtual bool interact(Player *p); //called when the player "clicks" on the entity, i.e. with talking
	virtual void draw(int x, int y); //if set to nullptr, no sprite will be diplayed
	virtual void collide(Entity *e);
	virtual bool collides(Entity *e);
	World *world = nullptr;
	virtual void save();
	virtual void load();
};

#endif