#include "Item.h"

Hashmap<string, Item*> items;
Stack<Item*> itemPools[4];

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

void fillPool(int type)
{
	int index = type - 1;
	List<Item*> list = items.values();
	while(list.length()>0)
	{
		int i = rand() % list.length();
		Item * item = list[i];
		list.removeAt(i);
		if (item->type == type)
		{
			itemPools[index].push(item);
		}
	}
}