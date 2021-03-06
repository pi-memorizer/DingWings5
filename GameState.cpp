#include "GameState.h"
#include "Entity.h"
#include "Player.h"
#include "World.h"
#include "Sprite.h"
#include "Event.h"
#include "Animation.h"
#include "Revengine.h"
#include "Shader.h"
#include "Item.h"

int timeLeft = MAX_TIME_LEFT;
int lives = MAX_LIVES;

GameState::GameState(Player *player)
{
	p = player;
	a = true;
	b = true;
	up = true;
	down = true;
	left = true;
	right = true;
}

void GameState::startMenu()
{

}

void GameState::endMenu()
{
	a = getKey(p, KEY_A);
	b = getKey(p, KEY_B);
	up = getKey(p, KEY_UP);
	down = getKey(p, KEY_DOWN);
	left = getKey(p, KEY_LEFT);
	right = getKey(p, KEY_RIGHT);
}

WorldState::WorldState(Player *player) : GameState(player)
{
	if (player->id == 0) timeLeft = MAX_TIME_LEFT;
	List<Item *> list = items.values();
	for (int i = 0; i < 4; i++)
	{
		while (itemPools[i].length() < 9) {
			int index = rand() % list.length();
			while (list[index]->type != i + 1) index = rand() % list.length();
			itemPools[i].push(list[index]);
		}
	}
	score = 0;
	for (int i = 0; i < 4; i++) itemTimes[i] = ITEM_TIME * 3 / 2 - ITEM_TIME*(rand() % 100) / 100;
	lives = MAX_LIVES;
}

void WorldState::draw()
{
	setDrawColor(0xFF, 0xFF, 0xFF, 255);
	clearScreen();
	sDefault();
	for (int i = 0; i < animations.length(); i++)
	{
		if (!animations[i]->isDone()) animations[i]->draw();
	}
	World *world = worlds[p->getWorldID()];
	for (int i = p->getCameraCenterX() - WIDTH / 2 - TILE_SIZE * 2 - getPaddingX(); i <= p->getCameraCenterX() + WIDTH / 2 + TILE_SIZE * 2 + getPaddingX(); i += TILE_SIZE)
	{
		for (int j = p->getCameraCenterY() - HEIGHT / 2 - TILE_SIZE * 2; j <= p->getCameraCenterY() + HEIGHT / 2 + TILE_SIZE * 2; j += TILE_SIZE)
		{
			int index = world->getLower(safeDiv(i, TILE_SIZE), safeDiv(j, TILE_SIZE));
			if (tileset[index] != nullptr&&index != 0) tileset[index]->draw(getOnscreenX(p, safeDiv(i, TILE_SIZE)*TILE_SIZE), getOnscreenY(p, safeDiv(j, TILE_SIZE)*TILE_SIZE));
		}
	}
	List<Entity*> &entities = worlds[p->getWorldID()]->entities;

	for (int i = 0; i < numPlayers; i++)
	{
		if (p->getWorldID() == players[i]->getWorldID()) players[i]->draw(getOnscreenX(p, players[i]->x), getOnscreenY(p, players[i]->y));
	}
	for (int i = p->getCameraCenterX() - WIDTH / 2 - TILE_SIZE * 2 - getPaddingX(); i <= p->getCameraCenterX() + WIDTH / 2 + TILE_SIZE * 2 + getPaddingX(); i += TILE_SIZE)
	{
		for (int j = p->getCameraCenterY() - HEIGHT / 2 - TILE_SIZE * 2; j <= p->getCameraCenterY() + HEIGHT / 2 + TILE_SIZE * 2; j += TILE_SIZE)
		{
			int index = world->getUpper(safeDiv(i, TILE_SIZE), safeDiv(j, TILE_SIZE));
			if (tileset[index] != nullptr&&index != 0) tileset[index]->draw(getOnscreenX(p, safeDiv(i, TILE_SIZE)*TILE_SIZE), getOnscreenY(p, safeDiv(j, TILE_SIZE)*TILE_SIZE));
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (itemTimes[i] >= ITEM_TIME)
			setDrawColor(0, 0, 255, 255);
		else
			setDrawColor(0, 255, 0, 255);
		Rect r;
		r.x = 2 * TILE_SIZE+2+i*TILE_SIZE;
		r.y = TILE_SIZE+2;
		r.h = 2;
		int w = itemTimes[i] * 28 / ITEM_TIME;
		if (w > 28) w = 28;
		if (w < 0) w = 0;
		r.w = w;
		fillRect(&r);
	}
	worlds[p->getWorldID()]->draw(p);
}

void attemptMove(Player *p, int dx, int dy, int speed)
{
	for (int i = 0; i < speed; i++)
	{
		p->x += dx;
		p->y += dy;
		if (worlds[p->getWorldID()]->collides(p->x, p->y, p->width, p->height))
		{
			p->x -= dx;
			p->y -= dy;
			break;
		}
		List<Entity*> list = worlds[p->getWorldID()]->entities;
		for (int i = 0; i < list.length(); i++)
		{
			if (list[i]->collides(p))
			{
				p->x -= dx;
				p->y -= dy;
				break;
			}
		}
	}
}

bool firstWithWorldId(int worldId, int playerId)
{
	for (int i = 0; i < numPlayers; i++)
	{
		if (getPlayer(i)->getWorldID() == worldId)
			return i == playerId;
	}
	return false;
}

int clamp(int value, int min, int max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void WorldState::run()
{
	if (p->id == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			itemTimes[i]--;
			if (itemTimes[i] == 0)
			{
				childrenChoices[i] = nullptr;
				score -= 20;
				itemTimes[i] = ITEM_TIME * 3 / 2 - ITEM_TIME*((rand() % 100) / 100);
				lives--;
				if (lives == 0)
				{
					p->setState(new LoseState(p));
					return;
				}
			}
		}
		timeLeft--;
		if (timeLeft%SWITCH_TIME == 0&&timeLeft!=0)
		{
			p->pushState(new SwitchState(p,this));
			return;
		}
	}
	for (int i = 0; i < animations.length(); i++)
	{
		if (animations[i]->isDone())
		{
			animations.removeAt(i);
			i--;
		}
		else {
			animations[i]->run();
		}
	}
	startMenu();
	if (this == p->getState())
	{
		if (p->building)
		{
			int dx = 0;
			int dy = 0;
			int _dir = p->dir;
			if (getKey(p, KEY_UP)) dy--;
			if (getKey(p, KEY_DOWN)) dy++;
			if (getKey(p, KEY_LEFT)) dx--;
			if (getKey(p, KEY_RIGHT)) dx++;
			if (p->dir == 0 && dx > 0)
			{
			}
			else if (p->dir == 1 && dy < 0)
			{
			}
			else if (p->dir == 2 && dx < 0)
			{
			}
			else if (p->dir == 3 && dy > 0)
			{
			}
			else {
				if (dx > 0)
					p->dir = 0;
				else if (dy < 0)
					p->dir = 1;
				else if (dx < 0)
					p->dir = 2;
				else if (dy > 0)
					p->dir = 3;
			}
			if (_dir != p->dir) p->wait = 0;
			if ((dx != 0 || dy != 0) && p->wait >= 3) attemptMove(p, dx, dy, 2);
			else if (dx == 0 && dy == 0)
				p->wait = 0;
			p->wait++;
			int sight = 1;
			if (getKey(p, KEY_A) && !a)
			{
				bool mapInteract = false;
				if (p->dir == 0)
				{
					if (worlds[p->getWorldID()]->interact(p, safeDiv(p->x + p->width + sight, TILE_SIZE), safeDiv(p->y + p->height / 2, TILE_SIZE)))
						mapInteract = true;
				}
				else
					if (p->dir == 1)
					{
						if (worlds[p->getWorldID()]->interact(p, safeDiv(p->x + p->width / 2, TILE_SIZE), safeDiv(p->y - sight - 1, TILE_SIZE)))
							mapInteract = true;
					}
					else
						if (p->dir == 2)
						{
							if (worlds[p->getWorldID()]->interact(p, safeDiv(p->x - sight - 1, TILE_SIZE), safeDiv(p->y + p->height / 2, TILE_SIZE)))
								mapInteract = true;
						}
						else
							if (p->dir == 3)
							{
								if (worlds[p->getWorldID()]->interact(p, safeDiv(p->x + p->width / 2, TILE_SIZE), safeDiv(p->y + p->height + sight, TILE_SIZE)))
									mapInteract = true;
							}
				if (!mapInteract)
				{
					//try interacting with entities
					List<Entity*> entities = worlds[p->getWorldID()]->entities;
					int x = 0;
					int y = 0;
					if (p->dir == 0)
					{
						x = p->x + p->width + sight;
						y = p->y + p->height / 2;
					}
					else if (p->dir == 1)
					{
						x = p->x + p->width / 2;
						y = p->y - sight;
					}
					else if (p->dir == 2)
					{
						x = p->x - sight;
						y = p->y + p->height / 2;
					}
					else if (p->dir == 3)
					{
						x = p->x + p->width / 2;
						y = p->y + p->height + sight;
					}
					for (int i = 0; i < entities.length(); i++)
					{
						Entity *e = entities[i];
						if (e->isAlive)
						{
							if (rectCollides(x, y, 2, 2, e->x, e->y, e->width, e->height))
							{
								if (e->interact(p))
								{
									break;
								}
							}
						}
					}
				}
			}
		}
		else {
			if (getKey(p, KEY_UP)&&!up)
			{
				p->y--;
			}
			if (getKey(p, KEY_DOWN)&&!down)
			{
				p->y++;
			}
			if (getKey(p, KEY_LEFT)&&!left)
			{
				p->x--;
			}
			if (getKey(p, KEY_RIGHT)&&!right)
			{
				p->x++;
			}
			p->x = clamp(p->x, 0, 1);
			p->y = clamp(p->y, 0, 2);
			if (getKey(p, KEY_A) && !a)
			{
				if (p->y == 2)
				{
					if (p->x == 0)
					{
						bool hasEverything = true;
						for (int i = 0; i < 5; i++) if (chosenParts[i] == nullptr) hasEverything = false;
						if (hasEverything)
						{
							int slot = -1;
							for (int i = 0; i < 5; i++)
							{
								if (packages[i].numStuff() == 0)
								{
									slot = i;
									break;
								}
							}
							if (slot != -1)
							{
								packages[slot].wrapping = nullptr;
								for (int i = 0; i < 5; i++)
								{
									packages[slot].parts[i] = chosenParts[i];
								}
								for (int i = 0; i < 5; i++) chosenParts[i] = nullptr;
							}
							else {
								//todo give feedback
							}
						}
						else {
							//todo give feedback
						}
					}
					else {
						for (int i = 0; i < 5; i++) chosenParts[i] = nullptr;
					}
				}
				else {
					int index = p->x + p->y * 2;
					if(chosenParts[index+1]==nullptr&&itemPools[index].length()>0)
						chosenParts[index + 1] = itemPools[index].pop();
				}
			}
		}
	}
	p->run();
	if (lives == 0)
	{
		p->setState(new LoseState(p));
		return;
	}
	if (firstWithWorldId(p->getWorldID(), p->id))
	{
		worlds[p->getWorldID()]->run();
		List<Entity*> &entities = worlds[p->getWorldID()]->entities;
		for (int i = 0; i < entities.length(); i++)
		{
			if (entities[i]->isAlive)
			{
				entities[i]->run();
			}
			if (!entities[i]->isAlive)
			{
				delete entities.removeAt(i);
				i--;
			}
		}
	}
	endMenu();
}

StartState::StartState(Player *player) : GameState(player)
{
}
void StartState::draw()
{
	if ((frames / 30) % 2 == 0)
		titleGreen->draw(0, 0);
	else
		titleRed->draw(0, 0);
}
void StartState::run()
{
	int down = 0;
	for (int i = 0; i < numPlayers; i++)
	{
		if (getKey(players[i], KEY_A))
			down++;
	}
	if (down == 2)
	{
		inPlayState = true;
		p->setState(new WorldState(p));
		return;
	}
}

WinState::WinState(Player *player) : GameState(player)
{
	inPlayState = false;
}
void WinState::draw()
{

}
void WinState::run()
{
	int down = 0;
	for (int i = 0; i < numPlayers; i++)
	{
		if (getKey(players[i], KEY_A))
			down++;
	}
	if (down == 2)
	{
		p->popState();
		return;
	}
}

LoseState::LoseState(Player *player) : GameState(player)
{
	inPlayState = false;
}
void LoseState::draw()
{

}
void LoseState::run()
{
	int down = 0;
	for (int i = 0; i < numPlayers; i++)
	{
		if (getKey(players[i], KEY_A))
			down++;
	}
	if (down == 2)
	{
		p->setState(new StartState(p));
		return;
	}
}

SwitchState::SwitchState(Player *player, GameState * caller) : GameState(player)
{
	inPlayState = false;
	this->caller = caller;
	Player *other = players[0] == p ? players[1] : players[0];
	if (p->building)
	{
		p->building = false;
		other->building = true;
	}
	else {
		p->building = true;
		other->building = false;
	}
	int x = p->x;
	int y = p->y;
	p->x = other->x;
	p->y = other->y;
	other->x = x;
	other->y = y;
}
void SwitchState::draw()
{
	caller->draw();
	string s = "Switch!";
	for (int i = 0; i < s.length(); i++)
		tileset[512 + s[i]]->draw(64 + TILE_SIZE*i, 64, TILE_SIZE, TILE_SIZE);
}
void SwitchState::run()
{
	if (time == 60)
	{
		inPlayState = true;
		p->popState();
		return;
	}
	else {
		time++;
	}
}