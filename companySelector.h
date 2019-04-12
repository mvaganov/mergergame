#pragma once
#include "queue.h"
#include "tilegroup.h"
#include "tile.h"

class Game;

struct SelectionOption{
	bool visible;
	bool selectable;
	SelectionOption(bool selectable, bool visible)
		:visible(visible),selectable(selectable){}
	SelectionOption():visible(true),selectable(true){}
};

class CompanySelector
{
	Game * m_game;
	Vector2 loc;
	int index;
	Queue<TileGroup> * m_companies;
	Queue<SelectionOption> selectability;
public:
	CompanySelector(Game * g, Vector2 loc, Queue<TileGroup> * a_companies)
		:m_game(g),loc(loc),index(0),m_companies(a_companies)
	{
		while(selectability.size < m_companies->size){
			selectability.enqueue(SelectionOption());
		}
	}
	TileGroup * getSelectedCompany(){
		if(index < 0 || index >= m_companies->size)
			return 0;
		return &m_companies->list[index];
	}
	int getSelectedCompanyIndex(){
		return index;
	}
	void setSelectability(int index, SelectionOption option){
		selectability.list[index] = option;
	}
	void setSelectability(TileGroup * c, SelectionOption option){
		int index = m_companies->indexOf(*c);
		if(index >= 0)
			setSelectability(index, option);
	}
	void setSelectability(TileGroup * c, int numCompanies, SelectionOption option){
		for(int i = 0; i < numCompanies; ++i){
			setSelectability(&c[i], option);
		}
	}
	void setSelectabilityForAll(SelectionOption option){
		for(int i = 0; i < selectability.size; ++i){
			selectability.list[i] = option;
		}
	}
	bool changeSelection(int change){
		bool validSelection;
		SelectionOption * s;
		int originalIndex = index;
		do{
			index += change;
			if(m_companies->size > 0){
				while(index < 0)index += m_companies->size;
				while(index >= m_companies->size)index -= m_companies->size;
			}
			s = &selectability.list[index];
			validSelection = s->visible && s->selectable;
			if(index == originalIndex){
				break;
			}
			if(originalIndex <0 || originalIndex >= m_companies->size)
				originalIndex = index;
		}while(!validSelection);
		return validSelection;
	}
	bool ensureValidSelection()
	{
		if(index >= 0 && index < m_companies->size
		&& selectability.list[index].selectable
		&& selectability.list[index].visible)
			return true;
		index = -1;
		return changeSelection(1);
	}
	void clearSelection(){
		index = -1;
	}
	void draw(Graphics * g);
};