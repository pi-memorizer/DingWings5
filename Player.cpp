#include "Player.h"
#include "GameState.h"
#include "Sprite.h"
#include "Item.h"
#include "World.h"
#include <random>

Player **players = new Player*[0];
int numPlayers = 0;
Sprite **guy = nullptr;
bool inPlayState = false;
int score = 0;

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
	x = 64;
	y = 64;
	worldID = 0;
	width = 32;
	height = 8;
	texture = createTexture(WIDTH, HEIGHT);
	pushState(new StartState(this));
	cameraLocked = false;
	cameraX = (WIDTH-TILE_SIZE)/2;
	cameraY = (HEIGHT-TILE_SIZE)/2;
	if (id != 0)
	{
		building = false;
	}
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
	int i = 0;
	if (dir == 0) i = 3;
	if (dir == 1) i = 9;
	if (dir == 2) i = 6;
	if ((wait / 10) % 4 == 1) i += 1;
	if ((wait / 10) % 4 == 3) i += 2;
	assert(i < 12&&i>=0);
	return guy[i];
}

void Player::draw(int x, int y)
{
	if (building) { 
		getSprite()->draw(x, y);
		if (heldItem.numStuff() != 0) {
			if (heldItem.numStuff() == 1)
			{
				heldItem.wrapping->sprite->draw(x, y - 24);
			}
			else if (heldItem.numStuff() < 6) {
				int index = -1;
				for (int i = 0; i < 5; i++) if (heldItem.parts[i]->type == ITEM_BODY) index = i;
				if (index != -1)
				{
					heldItem.parts[index]->sprite2->draw(x, y - 24);
				}
			} else {
				heldItem.wrapping->sprite2->draw(x, y - 24);
			}
		}
	}
	else {
		
	}
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