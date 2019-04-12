#pragma once

#include "mem.h"

template <typename T>
struct Queue
{
	T * list;
	int allocated;
	int size;
	Queue():list(0),size(0),allocated(0){}
	void setSize(int a_size)
	{
		T * newArray = NEWMEM_ARR(T, a_size);
		if(list){
			for(int i = 0; i < size && i < a_size; ++i)
				newArray[i] = list[i];
			delete [] list;
		}
		list = newArray;
		allocated = a_size;
	}
	void setSizeUsed(int a_size)
	{
		if(allocated < a_size){
			NEWMEM_SOURCE_TRACE(setSize(a_size));
		}
		size = a_size;
	}
	void enqueue(T value)
	{
		if(size >= allocated)
		{
			NEWMEM_SOURCE_TRACE(setSize(size+10));
		}
		list[size] = value;
		size++;
	}
	void remove(int index)
	{
		for(int i = index+1; i < size; ++i)
			list[i-1] = list[i];
		size--;
	}
	T pop()
	{
		T returned = list[0];
		remove(0);
		return returned;
	}
	T peek(){
		return list[0];
	}
	void swap(int a, int b)
	{
		T temp = list[a];
		list[a] = list[b];
		list[b] = temp;
	}
	int indexOf(T const & value)
	{
		for(int i = 0; i < size; ++i)
		{
			if(value == list[i])
				return i;
		}
		return -1;
	}
	/** if templated to a pointer type, this will not work. use sortReferences() */
	void sort()
	{
		bool swapped;
		int max = size;
		do
		{
			swapped = false;
			for(int i = 1; i < max; ++i)
			{
				if(list[i] < list[i-1]){
					swap(i, i-1);
					swapped = true;
				}
			}
			max--;
		}
		while(swapped);
	}
	/** call this if sorting an array of pointers */
	void sortReferences()
	{
		bool swapped;
		int max = size;
		do
		{
			swapped = false;
			for(int i = 1; i < max; ++i)
			{
				if(*(list[i]) < *(list[i-1])){
					swap(i, i-1);
					swapped = true;
				}
			}
			max--;
		}
		while(swapped);
	}
	void reverse()
	{
		int half = size/2;
		for(int i = 0; i < half; ++i){
			swap(i, size-1-i);
		}
	}
	void release()
	{
		if(list)
		{
			DELMEM_ARR(list);
			list = 0;
			size = 0;
			allocated = 0;
		}
	}
	~Queue()
	{
		release();
	}
};
