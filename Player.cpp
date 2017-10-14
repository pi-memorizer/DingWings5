#include "Player.h"
#include "GameState.h"
#include "Sprite.h"
#include "Item.h"
#include "World.h"
#include <random>

Player **players = new Player*[0];
int numPlayers = 0;
Sprite **guy = nullptr;

Player *getPlayer(int id)
{
	int index = -1;
	for (int i = 0; i < numPlayers; i++)
	{
		if (players[i]->id == id)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		return nullptr;
	else
		return players[index];
}

void addPlayer(int id)
{
	int index = -1;
	for (int i = 0; i < numPlayers; i++)
	{
		if (players[i]->id == id)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		Player **buffer = new Player*[numPlayers + 1];
		for (int i = 0; i < numPlayers; i++)
		{
			buffer[i] = players[i];
		}
		buffer[numPlayers] = new Player(id);
		delete[] players;
		players = buffer;
		numPlayers++;
	}
}

void removePlayer(int id)
{
	int index = -1;
	for (int i = 0; i < numPlayers; i++)
	{
		if (players[i]->id == id)
		{
			index = i;
			break;
		}
	}
	if (index != -1)
	{
		delete players[index];
		Player **buffer = new Player*[numPlayers - 1];
		for (int i = 0, j = 0; i < numPlayers; i++)
		{
			if (i != index)
			{
				buffer[j] = players[i];
				j++;
			}
		}
		delete[] players;
		players = buffer;
		numPlayers--;
	}
}

Player::Player(int id) : Entity(nullptr,-0,0)
{
	this->id = id;
	x = -64;
	y = -64;
	worldID = 0;
	width = 16;
	height = 16;
	texture = createTexture(WIDTH, HEIGHT);
	pushState(new WorldState(this));
	cameraLocked = false;
	cameraX = 0;
	cameraY = 64;
}

Player::~Player()
{
	destroyTexture(texture);
}

GameState *Player::getState()
{
	if (states.length() > 0)
		return states.peek();
	else
		return nullptr;
}

void Player::setWorldID(int id)
{
	worldID = id;
	worlds[id]->enter(this);
}

int Player::getWorldID()
{
	return worldID;
}

void Player::pushState(GameState *state)
{
	states.push(state);
}

void Player::popState()
{
	if(states.length()>0)
		delete states.pop();
}

void Player::setState(GameState *state)
{
	while (states.length() > 0)
	{
		delete states.pop();
	}
	states.push(state);
}

Sprite *Player::getSprite()
{
	int i = dir;
	if ((wait / 10) % 2 == 1) i += 4;
	if ((dir == 1 || dir == 3) && (wait / 10) % 4 == 3) i += 4;
	return guy[i];
}

void Player::draw(int x, int y)
{
	getSprite()->draw(x, y);
}

void Player::run()
{

}

void Player::lockCamera()
{
	cameraLocked = true;
}

void Player::unlockCamera()
{
	cameraLocked = false;
	cameraX = x;
	cameraY = y;
}

int Player::getCameraCenterX()
{
	if (cameraLocked)
	{
		return x;
	}
	else {
		return cameraX;
	}
}

int Player::getCameraCenterY()
{
	if (cameraLocked)
	{
		return y;
	}
	else {
		return cameraY;
	}
}