#pragma once
#include "tile.h"
#include "queue.h"

class TileGroup
{
private:
	const char *name;
	int color;
	/** which of the 3 levels of expensiveness is this company */
	int tier;
	int stock;
	Queue<Tile*> * tiles;
public:
	static const int MAX_SHARES = 25;

	const char *getName(){return name;}
	int getTier(){return tier;}
	int getFreeShares(){return stock;}
	int getColor(){return color;}
	void addFreeShares(int a_value){stock+=a_value;}

	void setup(const char *companyName, int companyTier, int companyColor){
		name = companyName;
		tier = companyTier;
		color = companyColor;
	}

	bool operator<(TileGroup & other){
		return getSize() < other.getSize();
	}

	TileGroup();
	~TileGroup();

	bool isOnBoard(){
		return tiles->size > 0;
	}

	void addTile(Tile * t)
	{
		tiles->enqueue(t);
	}

	void absorb(TileGroup * c)
	{
		Tile * t;
		// nom, nom, nom, nom, nom, nom, ...
		for(int ti = 0; ti < c->tiles->size; ++ti){
			t = c->tiles->list[ti];
			t->setCo(this);
			tiles->enqueue(t);
		}
		// clear tile references from the merged (and now dead) company
		c->tiles->size = 0;
	}

	void draw(Graphics * g, const char *text, int color);

	int getSize(){
		return tiles->size;
	}

	static int priceTier(int size)
	{
		// 2,3,4,5,6-10,11-20,21-30,31-40,41+
		if(size < 2)		return -1;
		else if(size < 3)	return 0;
		else if(size < 4)	return 1;
		else if(size < 5)	return 2;
		else if(size < 6)	return 3;
		else if(size < 11)	return 4;
		else if(size < 21)	return 5;
		else if(size < 31)	return 6;
		else if(size < 41)	return 7;
		else				return 8;
	}

	static int pricingFor(int tier, int size){
		int t = priceTier(size)+tier;
		if(t < tier)t = tier;
		int prices[] = {2,3,4,5,6,7,8,9,10,11,12};
		return prices[t]*100;
	}

	int getShareCost()
	{
		return pricingFor(tier, getSize());
	}

	int getMajorityShareholderBonus(int firstOrSecondStakeholder){
		return getShareCost()*10/firstOrSecondStakeholder;
	}
	bool operator==(TileGroup & other) const
	{
		return this == &other;
	}
};
