#include "Item.h"
#include "GameState.h"

Hashmap<string, Item*> items;
List<Item*> itemPools[4];
Item* chosenParts[5];
PackageDeal packages[5];
PackageDeal heldItem;
Item* childrenChoices[4] = { nullptr };
int itemTimes[4] = { -1 };

int PackageDeal::numStuff()
{
	int num = 0;
	if (this->wrapping != nullptr) num++;
	for (int i = 0; i < 5; i++) if (this->parts[i] != nullptr) num++;
	return num;
}

Item::Item(int t)
{
	this->type = t;
}

Item* Item::_sprite(Sprite *s)
{
	sprite = s;
	return this;
}

Item* Item::_sprite2(Sprite *s)
{
	sprite2 = s;
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