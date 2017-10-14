#include "Entity.h"
#include "Player.h"
#include "Graphics.h"
#include "World.h"
#include "Sprite.h"

Entity::Entity(World *w, int _x, int _y)
{
	world = w;
	x = _x;
	y = _y;
}

void Entity::run()
{
}

bool Entity::interact(Player *p)
{
	return false;
}

void Entity::save()
{

}

void Entity::load()
{

}

void Entity::draw(int x, int y)
{
}
void Entity::collide(Entity *e)
{
}
bool Entity::collides(Entity *e)
{
	return false;
}