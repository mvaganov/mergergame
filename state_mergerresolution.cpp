#include "state_mergerresolution.h"
#include "game.h"
#include "state_placetile.h"
#include "state_endgame.h"
#include "mem.h"

void State_MergerResolution::init(Game * a_g)
{
	GameState::init(a_g);
	// place the tile down as a default tile
	tile->setCo(m_game->getDefaultCompany());
	// add the merging tile, and attached tiles, to the dominant company
	m_game->fillTilesHereAs(tile, tile->getCo(), dominantCompany, 0);
	// merge the companies
	TileGroup * c;
	for(int co = 0; co < m_companies->size; ++co){
		c = m_companies->list[co];
		// don't merge the dominant company into itself...
		if(c == dominantCompany)
			continue;
		// nom, nom, nom, nom, nom, nom, ...
		dominantCompany->absorb(c);
	}
	// if the merged company is 44 or more, flag the game as over
	if(dominantCompany->getSize() >= Game::MAX_COMPANY_SIZE){
		m_game->queueState(NEWMEM(State_EndGame("%s grew to 44+ after a merger", dominantCompany->getName())));
	}else{// otherwise, keep playing as though the tile was finally placed.
		// put next state on the state queue here also
		State_PlaceTile::nextState(m_game);
	}
	// ok, this should be done now...
	setDone(true);
	m_game->clearCompanySelection();
	DELMEM(m_companies);
}
