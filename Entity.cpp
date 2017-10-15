#include "Entity.h"
#include "Player.h"
#include "Graphics.h"
#include "World.h"
#include "Sprite.h"
#include "Item.h"

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

ButtonEntity::ButtonEntity(World *world, int x, int y, int sprite, int type) : Entity(world,x,y)
{
	this->sprite = sprite;
	this->type = type;
}
void ButtonEntity::draw(int x, int y)
{
	tileset[sprite + (value<0?0:16)]->draw(x, y);
}

void ButtonEntity::run()
{
	value--;
}


void addToPool(int type)
{
	if (itemPools[type - 1].length() == 9) return;
	List<Item*> list = items.values();
	int i = rand() % list.length();
	while (list[i]->type != type)
		i = rand() % list.length();
	itemPools[type - 1].push(list[i]);
}


bool ButtonEntity::interact(Player *p)
{
	addToPool(type);
	value = 20;
	return true;
}

bool ButtonEntity::collides(Entity *e)
{
	return rectCollides(e->x, e->y, e->width, e->height, x + 2, y + 16, 28, 16);
}