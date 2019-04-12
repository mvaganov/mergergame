#include "board.h"
#include "tile.h"
#include "tilegroup.h"
#include "mem.h"
#include <stdlib.h>

// used for forcing draw patters for debugging. RIGGED_GAME is set in tile.h
#ifdef RIGGED_GAME
int fixedOrderPulled = 0;
int fixedOrderMax = 6;
int fixedW = 5, fixedH = 10;
int fixedOrderStart = 'a';
const char *fixedOrder[] ={
	"     ",
	"     ",
	"     ",
	" behd",
	"  g a",
	" cf  ",
	"    v",
	"qrstu",
	"ponml",
	"  ijk",
};
#endif

Board::Board(int width, int height):width(width),height(height){
	int total = width*height;
	map = NEWMEM_ARR(Tile, total);
	unusedTiles = NEWMEM(Queue<Tile*>());
	Tile * t;
	for(int row = 0; row < height; ++row)
	{
		for(int col = 0; col < width; ++col)
		{
			t = getTile(col, row);
			t->setPosition(Vector2(col, row));
			unusedTiles->enqueue(t);
		}
	}
}

Board::~Board(){
	DELMEM_ARR(map);
	DELMEM(unusedTiles);
}


Tile * Board::getRandomUnusedTile()
{
	if(unusedTiles->size == 0)
		return 0;
	int r = -1;
	Tile * t;
#ifdef RIGGED_GAME
	for(int row = 0; row < fixedH && r < 0; ++row){
		for(int col = 0; col < fixedW && r < 0; ++col){
			if(fixedOrder[row][col] == fixedOrderPulled+fixedOrderStart){
				t = getTile(col, row);
				r = unusedTiles->indexOf(t);
				fixedOrderPulled++;
			}
		}
	}
	if(r < 0)
#endif
	r = rand() % unusedTiles->size;
	unusedTiles->swap(r, 0);
	t = unusedTiles->pop();
	if(t->isPlaced()){
		int i=0;i=1/i;
	}
	return t;
}

void Board::draw(Graphics * g)
{
	int total = height*width;
	for(int i = 0; i < total; ++i){
		map[i].draw(g);
	}
}
