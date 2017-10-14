#ifndef ITEM_H
#define ITEM_H
#include "Revengine.h"

//Holds data about items

//The different Item categories, expressed with values that are exponents of two (bit flags)
#define ITEM_EYE 1
#define ITEM_BODY 2
#define ITEM_LIMB 3
#define ITEM_MOUTH 4
#define ITEM_ACCESSORY 5

///////////////////////////////////////////////////////
//IMPORTANT IDEA                                     //
//                                                   //
//Items can be initialized using the following       //
//syntax, using the functions that start with        //
//underscores:                                       //
//                                                   //
//Item *i = addItem("Apple")                         //
//          ->_flags(ITEM_FOOD|ITEM_OTHER_CATEGORY)  //
//          ->_sprite(spriteForInventory);           //
//                                                   //
///////////////////////////////////////////////////////

//Generic item class that holds the item's sprite, internal name, display name, and categories
class Item
{
public:
	Sprite *sprite = nullptr;
	int type;
	Item(int t);
	Item* _sprite(Sprite *sprite);
};

//a map of all the items in use
//all items created with the addItem function will be added automatically to this
//access items via their internal name (which can be different than the display name,
//especially if multiple items have the same display name)
extern Hashmap<string, Item*> items;

//creates an item given a certain name and adds it to the item list
Item* addItem(string name, int type);

#endif