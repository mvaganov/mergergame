#include "companySelector.h"
#include "game.h"
#include "cli.h"

void CompanySelector::draw(Graphics * g){
	g->move(loc.y, loc.x);
	g->resetColor();
	g->printf("     Company siz stock cost($) market cap");
	TileGroup * c;
	int color, marketcap;
	for(int i = 1; i < m_companies->size; ++i)
	{
		c = &m_companies->list[i];
		color = selectability.list[i].visible
			?(selectability.list[i].selectable
				?c->getColor()
				:CLI::COLOR::DARK_GRAY)
			:CLI::COLOR::BLACK;
		g->setColor(color, CLI::COLOR::DEFAULT);
		if(index == i){
			m_game->flashColor(1,g);
		}
		marketcap = (TileGroup::MAX_SHARES - c->getFreeShares()) * c->getShareCost();
		g->move(loc.y+i, loc.x);
		g->printf("%c%11s %2d  %3d  %5d   %10d", 
			c->getSize()?' ':'?', c->getName(), c->getSize(), c->getFreeShares(), c->getShareCost(), marketcap);
	}
}
