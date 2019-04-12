#include "tile.h"
#include "tilegroup.h"

const char *Tile::DEFAULT_UNPLACED = "......";
const char *Tile::DEFAULT_PLACEMENT = "######";

void Tile::draw(Graphics * g, const char *text, int color)
{
	int x = position.x * DRAW_WIDTH, y = position.y * DRAW_HEIGHT;
	if(color == CLI::COLOR::DEFAULT)
		g->resetColor();
	else
		g->setColor(color & 0xf, (color >> 4) & 0xf);
	g->drawText(text, x, y, DRAW_WIDTH, DRAW_HEIGHT, true);
}

void Tile::draw(Graphics * g)
{
	if(company){
		draw(g, company->getName(), company->getColor());
	}else{
		draw(g, DEFAULT_UNPLACED, CLI::COLOR::DEFAULT);
	}
}
