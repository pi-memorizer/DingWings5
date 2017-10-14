#include "Revengine.h"
#include "Player.h"
#include "Sprite.h"
#include "Event.h"
#include "GameState.h"

#ifdef MOBILE
string to_string(int value);
#endif

/*
better dynamic memory handling (allocators attached to states?)
pushing states and single time critical code inside scripts
*/

int frames = 0;
Hashmap<string, int> flags;
int screenWidth = WIDTH, screenHeight = HEIGHT;
bool prevDelay = true;

bool theLoop()
{
	Event e;
	while (getEvent(&e))
	{
		if (e.type == EVENT_QUIT)
		{
			return false;
		}
	}

	if (prevDelay)
	{
		setDrawColor(0, 0, 0, 255);
		clearScreen();

		int width;
		int height;
		getWindowSize(&width, &height);
		float wRat = width / (float)screenWidth;
		float hRat = height / (float)screenHeight;
		if (numPlayers > 0)
		{
			Rect rect;
			if (wRat < hRat)
			{
				int newHeight = (int)(width / (float)WIDTH * (float)HEIGHT);
				rect.h = newHeight;
				rect.w = width;
				rect.x = 0;
				rect.y = (height - newHeight) / 2;
			}
			else
			{
				int newWidth = (int)(height / (float)HEIGHT * (float)WIDTH);
				rect.w = newWidth;
				rect.h = height;
				rect.x = (width - newWidth) / 2;
				rect.y = 0;
			}
			startDrawingPlayer(0);
			players[0]->getState()->draw();
			startDrawingPlayer(-1);
			drawTexture(players[0]->texture, &rect);
		}
		display();
	}
	for (int i = 0; i < numPlayers; i++)
	{
		players[i]->getState()->run();
	}

	prevDelay = delay(16);
	if (!prevDelay)
	{
		debug("Frame skipped");
	}
	frames++;
	return true;
}

int getOnscreenX(Player *p, int x)
{
	if (p->cameraLocked)
	{
		return WIDTH / 2 - TILE_SIZE / 2 + (x - p->x);
	}
	else {
		return WIDTH / 2 - TILE_SIZE / 2 + (x - p->cameraX);
	}
}

int getOnscreenY(Player *p, int y)
{
	if (p->cameraLocked)
	{
		return HEIGHT / 2 - TILE_SIZE / 2 + (y - p->y);
	}
	else {
		return HEIGHT / 2 - TILE_SIZE / 2 + (y - p->cameraY);
	}
}

int getFlag(string s)
{
	if (flags.contains(s))
	{
		return flags[s];
	}
	else return 0;
}

void setFlag(string s, int v)
{
	if (flags.contains(s))
	{
		flags[s] = v;
	}
	else {
		flags.add(s, v);
	}
}