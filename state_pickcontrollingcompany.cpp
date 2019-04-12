#include "state_pickcontrollingcompany.h"
#include "state_placetile.h"
#include "game.h"
#include "tilegroup.h"

#include "cli.h"

void State_PickControllingCompany::init(Game * g){
	GameState::init(g);
	// select the companies from this list
	m_game->selectOneOfThese(this->pickWinner);
}

void State_PickControllingCompany::processInput(int key){
	//TileGroup * c;
	int dir = 0;
	switch(key)
	{
	case CLI::KEY::UP:		m_game->changeCompanySelection(-1);	break;
	case CLI::KEY::DOWN:	m_game->changeCompanySelection(+1);	break;
	case '\n':
	case '\r':
		// the dominant company was chosen, no need to keep this list now.
		DELMEM(pickWinner);
		State_PlaceTile::mergerSetup(m_game, t, involvedCompanies, m_game->getSelectedCompany());
		setDone(true);
		m_game->clearCompanySelection();
		break;
	}
}

void State_PickControllingCompany::draw(Graphics * g)
{
	TileGroup * c = m_game->getSelectedCompany();
	t->draw(g, c->getName(), m_game->flashColor(0,g));
	c->draw(g, c->getName(), m_game->flashColor(0,g));
}
