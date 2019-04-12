#include "state_endgame.h"
#include "game.h"
#include "queue.h"
#include "player.h"
#include "cli.h"

void State_EndGame::init(Game * g){
	GameState::init(g);
	Player * p;
	for(int i = 0; i < g->getPlayerCount(); ++i)
	{
		p = g->getPlayer(i);
		NEWMEM_SOURCE_TRACE(stats.enqueue(EndGameStats(p, p->getCash(), p->getValueOfAllStock())));
	}
	stats.sort();
	stats.reverse();
}

void State_EndGame::draw(Graphics * g)
{
	g->setColor(CLI::COLOR::DARK_GRAY);
	g->fillRect(5, 5, 40, 15, '#');
	g->fillRect(6, 8, 38, 10, ' ');
	m_game->flashColor(3,g);
	g->move(6,7);
	g->printf(this->reasonTheGameEnded, detail);
	g->resetColor();
	for(int i = 0; i < stats.size; ++i)
	{
		g->move(9+i,10);
		g->printf("$%10d %s", stats.list[i].worth, stats.list[i].p->getName());
	}
	m_game->flashColor(4,g);
	g->move(19,10);
	g->printf("(escape to quit)");
}

void State_EndGame::processInput(int key)
{
}
