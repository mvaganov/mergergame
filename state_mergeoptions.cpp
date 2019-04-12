#include "state_mergeoptions.h"
#include "vector2.h"
#include "game.h"
#include "state_buystock.h"
#include "cli.h"

#include <stdio.h>// sprintf_s

const char *State_MergeOptions::getName(){
	static char TEXT[256];
	#ifndef __STDC_WANT_SECURE_LIB__
	sprintf(TEXT,
#else
	sprintf_s(TEXT,sizeof(TEXT), 
#endif
		", let %s resolve %d %s share%s\n(up/down/left/right/enter)",
		p->getName(), s.amount, s.company->getName(), ((s.amount>1)?"s":""));
	return TEXT;
}

void State_MergeOptions::draw(Graphics * g)
{
	Vector2 loc(0, 20);
	bool opt;
	// <name> stock: <amount>
	g->move(loc.y, loc.x);
	g->printf("%s's %d %s stock", p->getName(), s.amount, s.company->getName());

	// sell at <value>:  - <amount> + ($<value>)
	opt = uiState == 0;
	if(opt)m_game->flashColor(1,g);
	else g->resetColor();
	g->move(loc.y+1, loc.x);
	g->printf(
		"%c %2d %c sell at %d ($%d)", 
		((opt && sell > 0)?'-':' '), 
		sell, 
		((opt && sell < s.amount)?'+':' '), 
		s.company->getShareCost(), 
		sell*s.company->getShareCost());

	opt = uiState == 1;
	if(opt)m_game->flashColor(1,g);
	else g->resetColor();
	// trade for <name>: - <amount> + (<amount> shares of <name>, $<value>)
	int newCompanyCost = boss->pricingFor(boss->getTier(), newCompanySize);
	g->move(loc.y+2, loc.x);
	g->printf(
		"%c %2d %c trade for %s: (%d shares, $%d)",
		((opt && trade > 0)?'-':' '), 
		trade,
		((opt && trade < s.amount)?'+':' '), 
		boss->getName(), trade/2, (trade/2)*newCompanyCost);

	opt = uiState == 2;
	if(opt)m_game->flashColor(1,g);
	else g->resetColor();
	// keep shares:      - <amount> +
	g->move(loc.y+3, loc.x);
	g->printf(
		"%c %2d %c keep %s shares",
		((opt && keep > 0)?'-':' '), 
		keep,
		((opt && keep < s.amount)?'+':' '),
		s.company->getName());

	tile->draw(g, Tile::DEFAULT_PLACEMENT, m_game->flashColor(0,g));
	s.company->draw(g, s.company->getName(), m_game->flashColor(0,g));
}

void State_MergeOptions::init(Game * a_g){
	GameState::init(a_g);
	m_game->drawInventoryForPlayerThisState(p);
}

void State_MergeOptions::release(){
	// pay the shareholder bonus
	p->addCash(bonus);
	// if shares were traded
	if(trade > 0){
		// give shares for trade
		p->addStock(s.company, -trade, m_game);
		// get shares from trade
		p->addStock(boss, trade/2, m_game);
	}
	// give shares for selling
	p->addStock(s.company, -sell, m_game);
	// get cash for the sold shares
	p->addCash(sell*s.company->getShareCost());
}

void State_MergeOptions::processInput(int key){
	int change = 0;
	switch(key){
	case CLI::KEY::UP:		uiState--;	break;
	case CLI::KEY::DOWN:	uiState++;	break;
	case CLI::KEY::LEFT:
		switch(uiState){
		case 0:	if(sell > 0)	{sell--;	trade++;}	break;
		case 1:	if(trade > 0)	{trade--;	keep++;}	break;
		case 2:	if(keep > 0)	{keep--;	sell++;}	break;
		}
		break;
	case CLI::KEY::RIGHT:
		switch(uiState){
		case 0:	if(trade > 0 || keep > 0)	{(trade > 0)?--trade:--keep;	sell++;}	break;
		case 1:	if((sell > 0 || keep > 0) && boss->getFreeShares() > (trade/2))
											{(sell > 0)?--sell:--keep;		trade++;}	break;
		case 2:	if(sell > 0 || trade > 0)	{(sell > 0)?--sell:--trade;		keep++;}	break;
		}
		break;
	case ' ':
	case '\n':
	case '\r':
		setDone(true);
		break;
	}
	while(uiState <  0)uiState += 3;
	while(uiState >= 3)uiState -= 3;
}
