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
#include "PhraseBattle.h"

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

Phrase testPhrases1[6] = {
	Phrase("<c00FF00>Y<c000000>ou're <c00FF00>a<c000000> <c00FF00>r<c000000>abbit",0,1),
	Phrase("<c00FF00>T<c000000>hat <c00FF00>f<c000000>luffy <c00FF00>t<c000000>ail <c00FF00>l<c000000>ooks <c00FF00>c<c000000>ute",0,2),
	Phrase("<c00FF00>S<c000000>orry, <c00FF00>o<c000000>ut <c00FF00>o<c000000>f <c00FF00>c<c000000>arot <c00FF00>c<c000000>ake",0,3),
	Phrase("<c00FF00>M<c000000>onty <c00FF00>P<c000000>ython <c00FF00>d<c000000>id <c00FF00>y<c000000>ou <c00FF00>b<c000000>etter",0,4),
	Phrase("<c00FF00>T<c000000>witch <c00FF00>t<c000000>witch <c00FF00>t<c000000>witch <c00FF00>t<c000000>witch <c00FF00>t<c000000>witch",0,5),
	Phrase("<c00FF00>G<c000000>o <c00FF00>b<c000000>ack <c00FF00>t<c000000>o <c00FF00>y<c000000>our <c00FF00>o<c000000>wn <c00FF00>g<c000000>ame",0,6)
};

void battleTest(Player*p, int x, int y, Script *s)
{
	s->textBox("What you gotta say to me, punk?", true);
	int i = s->phraseBattle(testPhrases1, 6);
	if (i < 30)
	{
		s->textBox("Is that all you got?", false);
	}
	else if (i >= 100)
	{
		s->textBox("Ouch, that one stung!", false);
	}
	else {
		s->textBox("Meh.", false);
	}
}

void nothing() {} //run variant
bool nothing(Player *p, int x, int y) { //interact variant
	return false;
}
bool nothing(Player *p) //interact entity variant
{
	return true;
}
void enterNothing(Player *p)
{
}

void testWorldLighting(Player *p)
{
	List<Light> lights;
	lights.add(Light(32.0F, 32.0F, 100.0F, 0.7, 0.7, 0.6));
	sLighting(p,0.2,0.2,0.4,lights);
	postProcess(p);
}

void init()
{
	createSound("door");

	//load sprites
	Sprite *guySheet = new Sprite("guy", 0, 0);
	guy = new Sprite*[12];
	for (int i = 0; i < 12; i++)
	{
		guy[i] = new Sprite(guySheet, 16 * (i % 4), 24 * (i / 4), 16, 24, 0, -8);
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
				tileset[i * 256 + j] = new Sprite(s, TILE_SIZE * (j % 16), TILE_SIZE * (j / 16), TILE_SIZE, TILE_SIZE, 0, 0);
			}
		}
		catch (int) {
		}
	}

	//add entities to worlds and stuff here preferably
	World *test = new StaticWorld("test", &enterNothing,&nothing, &testWorldLighting,&nothing);
	for(int i = 0; i < 5; i++)
		test->addEntity(new CoolGuy(test, -96, i*32));
	worlds.add(test);
	//Entity *testEntity = new Entity(test, -4, 9, 32, 32, nullptr, &nothing, &nothing);
	//test->addEntity(testEntity);
	addItem("null")
		->_flags(ITEM_FOOD | ITEM_FURNITURE)
		->_displayName("Nothing");

	load();

	//setBackgroundMusic("PossibleTheme");
}