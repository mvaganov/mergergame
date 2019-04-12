#include "state_buystock.h"
#include "state_placetile.h"
#include "game.h"
#include "state_cursor.h"
#include "mem.h"
#include "cli.h"

#include <stdio.h>// sprintf_s

const int MAX_STOCK_PURCHASE = 3;

char TEXT[256];

const char *State_BuyStock::getName(){
	if(stockBoughtThisTurn < MAX_STOCK_PURCHASE){
		int left = MAX_STOCK_PURCHASE-stockBoughtThisTurn;
#ifndef __STDC_WANT_SECURE_LIB__
	sprintf(TEXT,
#else
	sprintf_s(TEXT,sizeof(TEXT), 
#endif
		", purchase up to %d more stock%s\n(up/down/enter, space to skip)",
			left, ((left>1)?"s":""));
	}else{
#ifndef __STDC_WANT_SECURE_LIB__
		sprintf(TEXT,
#else
		sprintf_s(TEXT,sizeof(TEXT), 
#endif
			" is done with stock purchases\n(enter)");
	}
	return TEXT;
}

void State_BuyStock::nextState()
{
	m_game->clearCompanySelection();
	m_game->queueState(NEWMEM(State_Cursor));
	setDone(true);
}

void State_BuyStock::init(Game * g)
{
	GameState::init(g);
	stockBoughtThisTurn = 0;
	// if no purchasable was found, set the flag that disallows stock purchase
	if(!m_game->selectPurchasableStock(m_game->getCurrentPlayer())){
		stockBoughtThisTurn = MAX_STOCK_PURCHASE;
	}
}

void State_BuyStock::processInput(int key){
	Player * p = m_game->getCurrentPlayer();
	TileGroup * c = m_game->getSelectedCompany();
	int stockPrice = 0;
	switch(key)
	{
	case CLI::KEY::UP:		m_game->changeCompanySelection(-1);	break;
	case CLI::KEY::DOWN:	m_game->changeCompanySelection(+1);	break;
	case '\n':
	case '\r':
		if(stockBoughtThisTurn >= MAX_STOCK_PURCHASE)
		{
			nextState();
			break;
		}
		stockPrice = c->getShareCost();
		if(c->getFreeShares() > 0 && p->getCash() >= stockPrice)
		{
			p->addStock(c, 1, m_game);
			p->addCash(-stockPrice);
			stockBoughtThisTurn++;
		}
		// if no purchasable was found, set the flag that disallows stock purchase
		if(!m_game->selectPurchasableStock(p)){
			stockBoughtThisTurn = MAX_STOCK_PURCHASE;
		}
		break;
	case ' ':
		nextState();
		break;
	}
}
