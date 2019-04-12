#include "tilegroup.h"
#include "mem.h"

TileGroup::TileGroup():name(0),color(0),tier(0),stock(MAX_SHARES){
	tiles = NEWMEM(Queue<Tile*>());
}

TileGroup::~TileGroup(){
	DELMEM(tiles);
}

void TileGroup::draw(Graphics * g, const char *text, int color)
{
	Tile * t;
	for(int i = 0; i < tiles->size; ++i)
	{
		t = tiles->list[i];
		t->draw(g, name, color);
	}
}
