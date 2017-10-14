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

void Tongue::tonguePlacement()
{
	this->x = p->x + p->width / 2 - 1;
	this->y = p->y + p->height / 2 - 1;
	switch (p->dir)
	{
	case 0:
		x += p->width / 2;
		break;
	case 1:
		y -= p->height / 2;
		break;
	case 2:
		x -= p->width / 2;
		break;
	case 3:
		y += p->height / 2;
		break;
	default:
		break;
	}
}

Tongue::Tongue(World *world, Player *p) : Entity(world, 0, 0)
{
	this->p = p;
	width = 2;
	height = 2;
	power = -1;
	tonguePlacement();
}
void Tongue::run()
{
	tonguePlacement();
	int speed = 2;
	width = 2;
	height = 2;
	switch (p->dir)
	{
	case 0:
		width += size*speed;
		break;
	case 1:
		height += size*speed;
		y -= size*speed;
		break;
	case 2:
		width += size*speed;
		x -= size*speed;
		break;
	case 3:
		height += size*speed;
		break;
	default:
		break;
	}
	List<Entity*>&entities = world->entities;
	for (int i = 0; i < captures.length(); i++)
	{
		if (retracting)
		{
			switch (p->dir)
			{
			case 0:
				if (captures[i]->x - speed > 0)
					captures[i]->x -= speed;
				else
					captures[i]->x = 0;
				break;
			case 4:
				if (captures[i]->y + speed < 0)
					captures[i]->y += speed;
				else
					captures[i]->y = 0;
				break;
			case 5:
				if (captures[i]->x + speed < 0)
					captures[i]->x += speed;
				else
					captures[i]->x = 0;
				break;
			case 3:
				if (captures[i]->y - speed > 0)
					captures[i]->y -= speed;
				else
					captures[i]->y = 0;
				break;
			default:
				break;
			}
		}
		else {
			switch (p->dir)
			{
			case 0:
				captures[i]->x += speed;
				break;
			case 4:
				captures[i]->y -= speed;
				break;
			case 5:
				captures[i]->x -= speed;
				break;
			case 3:
				captures[i]->y += speed;
				break;
			}
		}
	}
	for (int i = 0; i < entities.length(); i++)
	{
		if (entities[i]->collides(this)&&entities[i]->isAlive&&entities[i]!=this&&entities[i]->power>=0)
		{
			captures.add(entities[i]);
			world->entities.remove(entities[i]);
			i--;
			Entity *e = captures[captures.length() - 1];
			e->x = e->x - x;
			e->y = e->y - y;
		}
	}
	if (retracting)
	{
		size--;
		if (size == 0)
		{
			if (captures.length() > 0)
			{
				p->power = captures[rand() % captures.length()]->power;
			}
			for (int i = 0; i < captures.length(); i++)
			{
				delete captures[i];
			}
			p->attacking = false;
			isAlive = false;
		}
	}
	else {
		size++;
		if (size == 32)
		{
			retracting = true;
		}
	}
}
void Tongue::draw(int x, int y)
{
	setDrawColor(255, 174, 201, 255);
	Rect r;
	r.x = x;
	r.y = y;
	r.w = width;
	r.h = height;
	fillRect(&r);
	for (int i = 0; i < captures.length(); i++)
	{
		captures[i]->draw(x + captures[i]->x, y + captures[i]->y);
	}
}

bool Tongue::collides(Entity *e)
{
	return isAlive&&rectCollides(x, y, width, height, e->x, e->y, e->width, e->height);
}

CoolGuy::CoolGuy(World *world, int x, int y) : Entity(world,x,y)
{
	width = 32;
	height = 32;
}
void CoolGuy::run()
{

}
void CoolGuy::draw(int x, int y)
{
	tileset[3]->draw(x, y);
}
bool CoolGuy::collides(Entity *e)
{
	return isAlive&&rectCollides(x, y, width, height, e->x, e->y, e->width, e->height);
}