#ifndef HGAUSHFAIUGJHASGHASD
#define HGAUSHFAIUGJHASGHASD

// Implementation of MinMaxHeap:
// M. D. Atkinson, J.R. Sack, N. Santoro, and T. Strothotte
// "Min-Max Heaps and Generalized Priority Queues"
// Communications of the ACM, October, 1986, 

template <class Item> class MinMaxHeap
{
public:

    MinMaxHeap(int estimated_max_size=64);
    ~MinMaxHeap();
	void Reset() { size = 0; }
	int GetSize() { return size; }

    void Insert(Item& item);

	Item& FindMin() { return items[1]; }
	void DeleteMin();
	Item& ExtractMin() { Item& item = FindMin(); DeleteMin(); return item; }

	Item& FindMax() { return (size<=2) ? items[size] : ((items[2] > items[3]) ? items[2] : items[3]); }
	void DeleteMax();
	Item& ExtractMax() { Item& item = FindMax(); DeleteMax(); return item; }


private:

    int size, size_max;
	Item* items;

    void SiftUp(int i);
    void SiftUpMin(int i);
    void SiftUpMax(int i);

    void SiftDown(int i);
    void SiftDownMin(int i);
    void SiftDownMax(int i);
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

// root:                 1
// last element:         size
// parent of i:          i/2
// grandparent of i:     i/4
// children of i:        2*i, 2*i+1
// grandchildren of i:   4*i, 4*i+1, 4*i+2, 4*i+3

#define MIN_MAX_HEAP_SWAP(i, j) { Item tmp = items[i]; items[i] = items[j]; items[j] = tmp; }


template <class Item> inline MinMaxHeap<Item>::MinMaxHeap(int estimated_max_size)
	: size(0), size_max(estimated_max_size)
{
	if (size_max < 2) size_max = 2;
	items = ((Item*) malloc(size_max*sizeof(Item))) - 1;
}

template <class Item> inline MinMaxHeap<Item>::~MinMaxHeap()
{
	free(items+1);
}

template <class Item> inline void MinMaxHeap<Item>::Insert(Item& item)
{
	if (size == size_max)
	{
		size_max *= 2;
		items = ((Item*) realloc(items+1, size_max*sizeof(Item))) - 1;
	}
	items[++size] = item;
	SiftUp(size);
}

template <class Item> inline void MinMaxHeap<Item>::DeleteMin()
{
	items[1] = items[size--];
	SiftDown(1);
}

template <class Item> inline void MinMaxHeap<Item>::DeleteMax()
{
    int k = (size<=2) ? size : ((items[2] > items[3]) ? 2 : 3);

    items[k] = items[size--];
    SiftDown(k);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

template <class Item> inline void MinMaxHeap<Item>::SiftUp(int i)
{
	int is_min_level, t, k=i/2;
	for (is_min_level=0, t=i; t>0; t>>=1) is_min_level = is_min_level ^ 1;

	if (is_min_level)
	{
		if (k && items[k] < items[i])
		{
			MIN_MAX_HEAP_SWAP(i, k);
			SiftUpMax(k);
		}
		else
		{
			SiftUpMin(i);
		}
	}
	else
	{
		if (k && items[k] > items[i])
		{
			MIN_MAX_HEAP_SWAP(i, k);
			SiftUpMin(k);
		}
		else
		{
			SiftUpMax(i);
		}
	}
}

template <class Item> inline void MinMaxHeap<Item>::SiftUpMin(int i)
{
	while ( 1 )
	{
		int k = i/4;

		if (k == 0 || !(items[i] < items[k])) break;
		MIN_MAX_HEAP_SWAP(i, k);
		i = k;
	}
}

template <class Item> inline void MinMaxHeap<Item>::SiftUpMax(int i)
{
	while ( 1 )
	{
		int k = i/4;
		if (k == 0 || !(items[i] > items[k])) break;
		MIN_MAX_HEAP_SWAP(i, k);
		i = k;
	}
}


template <class Item> inline void MinMaxHeap<Item>::SiftDown(int i)
{
	int is_min_level, t;
	for (is_min_level=0, t=i; t>0; t>>=1) is_min_level = is_min_level ^ 1;

	if (is_min_level) SiftDownMin(i);
	else              SiftDownMax(i);
}

template <class Item> inline void MinMaxHeap<Item>::SiftDownMin(int i)
{
	int k, t; // k is the index of the smallest (grand-)child

	while (4*i + 3 <= size)
	{
		// all four grandchildren are present - most frequent case
		k = 4*i;
		if (items[k] > items[4*i+1]) k = 4*i+1;
		if (items[k] > items[4*i+2]) k = 4*i+2;
		if (items[k] > items[4*i+3]) k = 4*i+3;

		if (!(items[k] < items[i])) return;

		MIN_MAX_HEAP_SWAP(i, k);
		if (items[k] > items[k/2]) MIN_MAX_HEAP_SWAP(k, k/2);
		i = k;
	}

	k = 2*i;
	if (k > size) return;
	if (2*i + 1 <= size)
	{
		if (items[k] > items[2*i+1]) k = 2*i + 1;
		for (t=4*i; t<=size; t++)
		{
			if (items[k] > items[t]) k = t;
		}
	}
	if (items[k] < items[i])
	{
		MIN_MAX_HEAP_SWAP(i, k);
		if (k >= 4*i && items[k] > items[k/2]) MIN_MAX_HEAP_SWAP(k, k/2);
	}
}

template <class Item> inline void MinMaxHeap<Item>::SiftDownMax(int i)
{
	int k, t; // k is the index of the largest (grand-)child

	while (4*i + 3 <= size)
	{
		// all four grandchildren are present - most frequent case
		k = 4*i;
		if (items[k] < items[4*i+1]) k = 4*i+1;
		if (items[k] < items[4*i+2]) k = 4*i+2;
		if (items[k] < items[4*i+3]) k = 4*i+3;

		if (!(items[k] > items[i])) return;

		MIN_MAX_HEAP_SWAP(i, k);
		if (items[k] < items[k/2]) MIN_MAX_HEAP_SWAP(k, k/2);
		i = k;
	}

	k = 2*i;
	if (k > size) return;
	if (2*i + 1 <= size)
	{
		if (items[k] < items[2*i+1]) k = 2*i + 1;
		for (t=4*i; t<=size; t++)
		{
			if (items[k] < items[t]) k = t;
		}
	}
	if (items[k] > items[i])
	{
		MIN_MAX_HEAP_SWAP(i, k);
		if (k >= 4*i && items[k] < items[k/2]) MIN_MAX_HEAP_SWAP(k, k/2);
	}
}




#endif



/*
// Test correctness of MinMaxHeap implementation

#include <stdio.h>
#include <stdlib.h>
#include "MinMaxHeap.h"

struct Item
{
	bool operator>(const Item& b) const { return (x > b.x); }
	bool operator<(const Item& b) const { return (x < b.x); }
	int x;
};

void Test(int iter_num, int MAX_SIZE)
{
	int* arr = new int[MAX_SIZE];
	int i, size = 0;

	MinMaxHeap<Item> pq(2);
	Item item;

	for ( ; iter_num>0; iter_num--)
	{
		int insert;
		if (size == 0) insert = 1;
		else if (size == MAX_SIZE) insert = 0;
		else insert = (rand() % 2);
		if (insert)
		{
			item.x = arr[size ++] = rand() % 100;
			pq.Insert( item );
		}
		else
		{
			int i_opt = 0;
			int max_flag = rand() % 2;
			if (max_flag)
			{
				for (i=1; i<size; i++)
				{
					if (arr[i_opt] < arr[i]) i_opt = i;
				}
				item = pq.FindMax();
				pq.DeleteMax();
			}
			else
			{
				for (i=1; i<size; i++)
				{
					if (arr[i_opt] > arr[i]) i_opt = i;
				}
				item = pq.FindMin();
				pq.DeleteMin();
			}
			if (item.x != arr[i_opt])
			{
				printf("Oops!\n");
				exit(1);
			}
			arr[i_opt] = arr[--size];
		}
	}
	printf("%d\n", size);

	delete [] arr;
}

int main()
{
	int seed;

	for (seed=1; ; seed++)
	{
		srand(seed);
		printf("seed=%d\n", seed);

		Test(10000, 500);
	}
}

*/