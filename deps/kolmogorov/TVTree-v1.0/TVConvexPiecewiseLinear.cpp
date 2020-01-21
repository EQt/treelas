/* Copyright 2014 Vladimir Kolmogorov vnk@ist.ac.at */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TVchain.h"
#include "block.h"
#include "MinMaxHeap.h"

struct Item
{
	//Item() {}
	//Item(double* _ptr) : ptr(_ptr) {}
	bool operator>(const Item& b) const { return ((*ptr) > (*b.ptr)); }
	bool operator<(const Item& b) const { return ((*ptr) < (*b.ptr)); }
	double* ptr;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void _SolveTVConvexPiecewiseLinear(int n, double* unaries, int* breakpoint_num_array, int breakpoint_num, double* w, double* solution)
{
	if (n <= 0) return;

	int i, k, total_breakpoint_num=0;
	if (breakpoint_num_array)
	{
		for (i=0; i<n; i++) total_breakpoint_num += breakpoint_num_array[i];
	}
	else total_breakpoint_num = n*breakpoint_num;

	MinMaxHeap<Item> pq(total_breakpoint_num);

	double* lambda_min = solution;
	double* lambda_max = new double[n-1];

	double a_left = 0, a_right = 0;
	for (i=0; ; i++)
	{
		// add unary
		int t = (breakpoint_num_array) ? breakpoint_num_array[i] : breakpoint_num;
		a_left += unaries[0];
		a_right += unaries[2*t];
		double tmp = unaries[0];
		for (k=1; k<=t; k++)
		{
			double diff = unaries[2*k] - tmp;
			tmp = unaries[2*k];
			unaries[2*k] = diff;
			Item c; c.ptr = &unaries[2*k-1];
			pq.Insert( c );
		}
		unaries += 2*t+1;

		// cut (pass message)
		double W = (i==n-1) ? 0 : w[i];

		if (a_right <= -W)
		{
			if (i == n-1) { solution[i] = BREAKPOINT_INFTY; break; }
			a_left = a_right = -W;
			pq.Reset();
			lambda_min[i] = lambda_max[i] = BREAKPOINT_INFTY;
			continue;
		}

		if (a_left >= W)
		{
			if (i == n-1) { solution[i] = -BREAKPOINT_INFTY; break; }
			a_left = a_right = W;
			pq.Reset();
			lambda_min[i] = lambda_max[i] = -BREAKPOINT_INFTY;
			continue;
		}

		double l_min = -BREAKPOINT_INFTY;
		double l_max = +BREAKPOINT_INFTY;

		if (a_left <= -W)
		{
			while (pq.GetSize() > 0)
			{
				double* p = (pq.FindMin()).ptr;
				l_min = p[0];
				a_left += p[1];
				if (a_left > -W)
				{
					p[1] = a_left - (-W);
					a_left = -W;
					break;
				}
				pq.DeleteMin();
			}
		}

		if (i == n-1) { solution[i] = l_min; break; }

		if (a_right >= W)
		{
			while (pq.GetSize() > 0)
			{
				double* p = (pq.FindMax()).ptr;
				l_max = p[0];
				a_right -= p[1];
				if (a_right < W)
				{
					p[1] = W - a_right;
					a_right = W;
					break;
				}
				pq.DeleteMax();
			}
		}

		lambda_min[i] = l_min;
		lambda_max[i] = l_max;
	}

	for (i-- ; i>=0; i--)
	{
		if (solution[i+1] < lambda_min[i]) solution[i] = lambda_min[i];
		else if (solution[i+1] > lambda_max[i]) solution[i] = lambda_max[i];
		else solution[i] = solution[i+1];
	}

	delete [] lambda_max;
}

void SolveTVConvexPiecewiseLinear(int n, double* unaries, int* breakpoint_num, double* w, double* solution)
{
	_SolveTVConvexPiecewiseLinear(n, unaries, breakpoint_num, 0, w, solution);
}

void SolveTVConvexPiecewiseLinear(int n, double* unaries, int breakpoint_num, double* w, double* solution)
{
	_SolveTVConvexPiecewiseLinear(n, unaries, NULL, breakpoint_num, w, solution);
}
