#include "state_pickunplacedcompany.h"
#include "game.h"
#include "state_placetile.h"
#include "state_buystock.h"
#include "mem.h"
#include "cli.h"

void State_PickUplacedCompany::draw(Graphics * g){
	TileGroup * c = m_game->getSelectedCompany();
	const char * text = c?c->getName():Tile::DEFAULT_PLACEMENT;
	selectedTile->draw(g, text, m_game->flashColor(0,g));
}

void State_PickUplacedCompany::init(Game * a_g){
	GameState::init(a_g);
	m_game->selectUnplacedCompany();
}

void State_PickUplacedCompany::processInput(int key){
	Player * p = m_game->getCurrentPlayer();
	TileGroup * c = m_game->getSelectedCompany();
	switch(key)
	{
	case CLI::KEY::UP:		m_game->changeCompanySelection(-1);	break;
	case CLI::KEY::DOWN:	m_game->changeCompanySelection(+1);	break;
	case '\n':
	case '\r':
		m_game->fillTilesHereAs(selectedTile, m_game->getDefaultCompany(), c, 0);
		// give founder stock bonus
		p->addStock(c, 1, m_game);
		m_game->clearCompanySelection();
		m_game->queueState(NEWMEM(State_BuyStock));
		setDone(true);
		break;
	}
}

void State_PickUplacedCompany::release(){
	m_game->clearCompanySelection();
}
