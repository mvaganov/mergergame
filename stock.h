#pragma once

class TileGroup;
class Player;

struct Stock
{
	TileGroup * company;
	Player * owner;
	int amount;
	int turnPurchased;
	Stock(TileGroup * company, int amount, int turnPurchased, Player * buyer)
		:company(company),owner(buyer),amount(amount),turnPurchased(turnPurchased){}
	Stock():company(0),owner(0),amount(0),turnPurchased(-1){}
	bool operator<(Stock & other){return amount < other.amount;}
};
