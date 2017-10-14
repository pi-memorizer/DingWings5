#include "Item.h"

Hashmap<string, Item*> items;

Item::Item(int t)
{
	this->type = t;
}

Item* Item::_sprite(Sprite *s)
{
	sprite = s;
	return this;
}

Item * addItem(string name, int type)
{
	if (items.contains(name))
	{
		debug("Duplicate item " << name);
		return items[name];
	}
	else {
		Item *i = new Item(type);
		items.add(name, i);
		return i;
	}
}