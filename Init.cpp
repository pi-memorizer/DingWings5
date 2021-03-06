#include "Revengine.h"
#include "Blocking.h"
#include "World.h"
#include "Player.h"
#include "Entity.h"
#include "Item.h"
#include "Sprite.h"
#include "IO.h"
#include "SoundSystem.h"
#include "GameState.h"
#include "Shader.h"
#include <ctime>

void testScript(Player*p, int x, int y, Script *s)
{
	s->textBox("Hello there", true);
	s->textBox("I'm a test", true);
	if (s->optionPane("Do you like me?", "Yes", "No") == "Yes")
	{
		s->textBox("Yay!", true);
	}
	else {
		s->textBox("You suck!", true);
	}
}

void battleTest(Player*p, int x, int y, Script *s)
{
	s->textBox("What you gotta say to me, punk?", true);
}


void checkPools()
{
	if (chosenParts[0] == nullptr) {
		List<Item*> list = items.values();
		int i = rand() % list.length();
		while (list[i]->type != ITEM_BODY)
			i = rand() % list.length();
		chosenParts[0] = list[i];
	}
	for (int i = 0; i < 4; i++)
	{
		if (childrenChoices[i] == nullptr)
		{
			List<Item*> list = items.values();
			childrenChoices[i] = list[rand() % list.length()];
		}
	}
}

void nothing() {} //run variant
bool nothing(Player *p, int x, int y) { //interact variant
	if (x == 7 && (y >= 3 && y <= 7))
	{
		if (heldItem.numStuff() > 1)
		{
			if (packages[y - 3].numStuff() == 0)
			{
				packages[y - 3] = heldItem;
				heldItem = PackageDeal();
			}
		}
		else if (heldItem.numStuff() == 1)
		{
			if (packages[y - 3].numStuff()>0 && packages[y - 3].wrapping == nullptr)
			{
				packages[y - 3].wrapping = heldItem.wrapping;
				heldItem.wrapping = nullptr;
			}
		}
		else {
			if (packages[y - 3].numStuff() > 0)
			{
				heldItem = packages[y - 3];
				packages[y - 3] = PackageDeal();
			}
		}
		return true;
	}
	if (x == 0 && (y >= 3 && y <= 5))
	{
		if (heldItem.wrapping == nullptr)
		{
			if (y == 3)
				heldItem.wrapping = items["redpaper"];
			else if (y == 4)
				heldItem.wrapping = items["greenpaper"];
			else if (y == 5)
				heldItem.wrapping = items["bluepaper"];
		}
		return true;
	}
	if (x == 0 && y == 7)
	{
		heldItem = PackageDeal();
	}
	if ((x >= 2 && x <= 5) && y == 1)
	{
		if (heldItem.numStuff() < 6) return false;
		if (childrenChoices[x - 2]->type == ITEM_WRAPPING)
		{
			if (heldItem.wrapping == childrenChoices[x - 2])
			{
				//todo add points
				score += 25;
			}
			else {
				//todo subtract points
				score -= 10;
				lives--;
			}
		}
		else {
			int index = -1;
			for (int i = 0; i < 5; i++) if (childrenChoices[x - 2]->type == heldItem.parts[i]->type) index = i;
			if (index != -1)
			{
				if (childrenChoices[x - 2] == heldItem.parts[index])
				{
					//todo add points
					score += 25;
				}
				else {
					//todo subtract points
					score -= 10;
					lives--;
				}
			}
		}
		itemTimes[x-2] = ITEM_TIME * 3 / 2 - ITEM_TIME*((rand() % 100) / 100);
		childrenChoices[x - 2] = nullptr;
		heldItem = PackageDeal();
		return true;
	}
	return false;
}
bool nothing(Player *p) //interact entity variant
{
	return true;
}
void enterNothing(Player *p)
{
}

Sprite *liveSprite;

void testWorldLighting(Player *p)
{
	checkPools();
	for (int i = 0; i < 5; i++)
	{
		if (chosenParts[i] != nullptr)
		{
			chosenParts[i]->sprite->draw(TILE_SIZE * 10, TILE_SIZE * 3);
		}
	}
	for (int i = 0; i < 5; i++)
	{
		if (packages[i].numStuff() != 0)
		{
			if (packages[i].wrapping != nullptr)
			{
				packages[i].wrapping->sprite2->draw(TILE_SIZE * 7, TILE_SIZE*(3 + i));
			}
			else {
				int index = -1;
				for (int j = 0; j < 5; j++) if (packages[i].parts[j]->type == ITEM_BODY) index = j;
				packages[i].parts[index]->sprite2->draw(TILE_SIZE * 7, TILE_SIZE*(3 + i));
			}
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if(childrenChoices[i]->sprite2!=nullptr)
			childrenChoices[i]->sprite2->draw(TILE_SIZE * (2 + i), 0, TILE_SIZE, TILE_SIZE);
		else
			childrenChoices[i]->sprite->draw(TILE_SIZE * (2+i), 0,TILE_SIZE,TILE_SIZE);
	}

	for (int i = 0; i < 4; i++)
	{
		if (itemPools[i].length() != 0) {
			Sprite *s = itemPools[i].peek()->sprite;
			s->draw(TILE_SIZE*(8 + 6 * (i % 2)) - s->xOffset, TILE_SIZE*(1 + 6 * (i / 2)) - s->yOffset);
		}
		tileset[512 + '0' + itemPools[i].length()]->draw(TILE_SIZE*(10 + 3 * (i % 2)), TILE_SIZE*(2 + 5 * (i / 2)), TILE_SIZE, TILE_SIZE);
	}
	{
		Player *t = players[0]->building ? players[1] : players[0];
		if (t->y == 2)
		{
			if (t->x == 1)
				tileset[256 + 30]->draw(WIDTH - TILE_SIZE, HEIGHT - 2 * TILE_SIZE);
			else tileset[256 + 30]->draw(WIDTH / 2, HEIGHT - 2 * TILE_SIZE);
		}
		else {
			tileset[256 + 30]->draw(TILE_SIZE*(9 + 6 * t->x), TILE_SIZE * (2 + 6 * t->y));
		}
	}
	List<Entity*> entities = worlds[p->getWorldID()]->entities;
	for (int i = 0; i < entities.length(); i++)
	{
		Entity *e = entities[i];
		assert(e != nullptr);
		if (e->isAlive)
		{
			e->draw(getOnscreenX(p, e->x), getOnscreenY(p, e->y));
		}
	}

	/*List<Light> lights;
	lights.add(Light(32.0F, 32.0F, 100.0F, 0.7, 0.7, 0.6));
	sLighting(p, 0.2, 0.2, 0.4, lights);
	postProcess(p, 0, 0, WIDTH / 2, HEIGHT- TILE_SIZE);*/
	List<Light> lights2;
	lights2.add(Light(9*32, 2*32, 35.0F, 0.4, 0.4, 0.7));
	lights2.add(Light(15 * 32, 2 * 32, 35.0F, 0.4, 0.4, 0.7));
	lights2.add(Light(9 * 32, 8 * 32, 35.0F, 0.4, 0.4, 0.7));
	lights2.add(Light(15 * 32, 8 * 32, 35.0F, 0.4, 0.4, 0.7));
	lights2.add(Light(12 * 32, 5 * 32, 100.0F, .3, .3, 0.2));
	for (int i = 0; i < lights2.length(); i++) lights2[i].flicker = .035F;
	sLighting(p, 0.7, 0.7, 0.8, lights2);
	postProcess(p, WIDTH / 2, 0, WIDTH / 2, HEIGHT - TILE_SIZE);

	string s = to_string(score);
	for (int i = 0; i < s.length(); i++)
	{
		tileset[512 + s[i]]->draw(WIDTH/2-s.length()*8+16*i, HEIGHT - TILE_SIZE, TILE_SIZE/2, TILE_SIZE/2);
	}
	if (lives > 0) for (int i = 0; i < lives; i++)
	{
		liveSprite->draw((WIDTH - lives * 16) / 2 + i*16, HEIGHT - 16);
	}
}

Sprite *titleGreen;
Sprite *titleRed;

void init()
{
	for (int i = 0; i < 5; i++)
		chosenParts[i] = nullptr;
	srand(time(0));
	createSound("door");
	//load sprites
	Sprite *guySheet = new Sprite("elfSheet", 0, 0);
	guy = new Sprite*[12];
	for (int i = 0; i < 12; i++)
	{
		bool a = (i / 3) == 0 || i / 3 == 3;
		guy[i] = new Sprite(guySheet, 32 * (i % 3), 32 * (i / 3), 32, 32, 0, i%3==0?-24:(a?-25:-26));
	}
	tileset = new Sprite*[65536];
	for (int i = 0; i < 65536; i++)
	{
		tileset[i] = nullptr;
	}
	for (int i = 0; i < 256; i++)
	{
		try {
			Sprite *s = new Sprite("tileset" + to_string(i + 1), 0, 0);
			for (int j = 0; j < 256; j++)
			{
				int t = TILE_SIZE;
				if (i == 2) t = 8;
				tileset[i * 256 + j] = new Sprite(s, t * (j % 16), t * (j / 16), t, t, 0, 0);
			}
		}
		catch (int) {
		}
	}
	liveSprite = new Sprite("lives", 0, 0);
	titleGreen = new Sprite("titleGreen", 0, 0);
	titleRed = new Sprite("titleRed", 0, 0);

	//add entities to worlds and stuff here preferably
	World *test = new StaticWorld("map", &enterNothing,&checkPools, &testWorldLighting,&nothing);
	worlds.add(test);
	for (int i = 1; i <= 4; i++) test->addEntity(new ButtonEntity(test, 32 + 32 * i, 9 * 32, 257 + i, i));
	//Entity *testEntity = new Entity(test, -4, 9, 32, 32, nullptr, &nothing, &nothing);
	//test->addEntity(testEntity);
	addItem("body1",ITEM_BODY)->_sprite(new Sprite("body1",0,0))->_sprite2(tileset[266+16]);
	addItem("body2", ITEM_BODY)->_sprite(new Sprite("body2", 0, 0))->_sprite2(tileset[267+16]);
	addItem("body3", ITEM_BODY)->_sprite(new Sprite("body3", 0, 0))->_sprite2(tileset[268+16]);
	addItem("body4", ITEM_BODY)->_sprite(new Sprite("body4", 0, 0))->_sprite2(tileset[265+16]);
	addItem("animeeyes", ITEM_EYE)->_sprite(new Sprite("animeeyes", 35, 28))->_sprite2(tileset[256 + 32+3]);
	addItem("jimmyrustlereyes", ITEM_EYE)->_sprite(new Sprite("jimmyrustlereyes", 34, 28))->_sprite2(tileset[256+32-1]);
	addItem("spidereyes", ITEM_EYE)->_sprite(new Sprite("spidereyes", 34, 30))->_sprite2(tileset[256 + 32+2]);
	addItem("unibroweyes", ITEM_EYE)->_sprite(new Sprite("unibroweyes", 34, 24))->_sprite2(tileset[256 + 32+1]);
	addItem("bow", ITEM_ACCESSORY)->_sprite(new Sprite("bow", 34, 8))->_sprite2(tileset[256 + 32+4]);
	addItem("tophat", ITEM_ACCESSORY)->_sprite(new Sprite("tophat", 54-20, 15-16))->_sprite2(tileset[256 + 32+5]);
	addItem("wizardhat", ITEM_ACCESSORY)->_sprite(new Sprite("wizardhat", 50-16, 8-11))->_sprite2(tileset[256 + 32+6]);
	addItem("antlers", ITEM_ACCESSORY)->_sprite(new Sprite("antlers", 45-12, 2-12))->_sprite2(tileset[256 + 32+9]);
	addItem("jimmyrustlermouth", ITEM_MOUTH)->_sprite(new Sprite("jimmyrustlermouth", 35, 40))->_sprite2(tileset[256 + 32+10]);
	addItem("moustache", ITEM_MOUTH)->_sprite(new Sprite("moustache", 44-10, 69-22))->_sprite2(tileset[256 + 32+11]);
	addItem("eldritchhorrormouth", ITEM_MOUTH)->_sprite(new Sprite("eldritchhorrormouth", 48, 66))->_sprite2(tileset[256 + 32+12]);
	addItem("lusciouslips", ITEM_MOUTH)->_sprite(new Sprite("lusciouslips", 52-18, 68-22))->_sprite2(tileset[256 + 32+13]);
	addItem("tentacles", ITEM_LIMB)->_sprite(new Sprite("tentacles", 39-5, 82-12));
	addItem("paws", ITEM_LIMB)->_sprite(new Sprite("paws", 44-11, 86-24));
	addItem("birdlegs", ITEM_LIMB)->_sprite(new Sprite("birdlegs", 49-13, 98-15));
	addItem("shit", ITEM_LIMB)->_sprite(new Sprite("shit", 36-2, 88-16));
	addItem("redpaper", ITEM_WRAPPING)->_sprite(tileset[265])->_sprite2(tileset[268]);
	addItem("greenpaper", ITEM_WRAPPING)->_sprite(tileset[266])->_sprite2(tileset[269]);
	addItem("bluepaper", ITEM_WRAPPING)->_sprite(tileset[267])->_sprite2(tileset[270]);
	for (int i = 265; i <= 270; i++) tileset[i]->yOffset -= 8;
	load();

	//setBackgroundMusic("PossibleTheme");
}