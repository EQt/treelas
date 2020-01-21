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

/*
#include <math.h>
void PrintPQ(MinMaxHeap<Item>& pq, double a_left, double b_left, double a_right, double b_right)
{
	int k, t = pq._currentSize;
	double** _array = new double*[t];
	for (k=0; k<t; k++) _array[k] = (pq._array[k+1]).ptr;
	bool flag;
	do
	{
		flag = false;
		for (k=0; k<t-1; k++)
		{
			if (*_array[k] > *_array[k+1]) { double* p = _array[k]; _array[k] = _array[k+1]; _array[k+1] = p; flag = true; }
		}
	} while (flag);
	printf("%f", a_left);
	for (k=0; k<t; k++)
	{
		double lambda = _array[k][0];
		printf(" [%f %f", a_left*lambda - b_left, lambda);
		a_left += _array[k][1];
		b_left += _array[k][2];
		printf(" %f] %f", a_left*lambda - b_left, a_left);
	}
	printf("\n");
	if (fabs(a_left - a_right) > 1e-10 || fabs(b_left - b_right) > 1e-10)
	{
		printf("(a,b): (%f,%f) vs (%f,%f)\n", a_left, b_left, a_right, b_right);
		getchar();
	}
}
*/
	
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void _SolveTVConvexPiecewiseQuadratic(int n, double* unaries, int* breakpoint_num_array, int breakpoint_num, double* w, double* solution)
{
	if (n <= 0) return;

	int i, k, total_breakpoint_num=0;
	if (breakpoint_num_array)
	{
		for (i=0; i<n; i++) total_breakpoint_num += breakpoint_num_array[i];
	}
	else total_breakpoint_num = n*breakpoint_num;

	MinMaxHeap<Item> pq(total_breakpoint_num + 2*n);

	double* lambda_min = solution;
	double* lambda_max = new double[n-1 + 6*n];
	double* buf = lambda_max + (n-1);

	double a_left = 0, b_left = 0, a_right = 0, b_right = 0;
	for (i=0; ; i++)
	{
		// add unary
		int t = (breakpoint_num_array) ? breakpoint_num_array[i] : breakpoint_num;
		a_left += unaries[0];
		b_left += unaries[1];
		a_right += unaries[3*t];
		b_right += unaries[3*t+1];
		double a_tmp = unaries[0], b_tmp = unaries[1];
		for (k=1; k<=t; k++)
		{
			double a_diff = unaries[3*k] - a_tmp;
			double b_diff = unaries[3*k+1] - b_tmp;
			a_tmp = unaries[3*k];
			b_tmp = unaries[3*k+1];
			unaries[3*k] = a_diff;
			unaries[3*k+1] = b_diff;
			Item c; c.ptr = &unaries[3*k-1];
			pq.Insert( c );
		}
		unaries += 3*t+2;

//printf("unary %d added\n", i); PrintPQ(pq, a_left, b_left, a_right, b_right);

		// cut (pass message)
		double W = (i==n-1) ? 0 : w[i];
		double l_min = -BREAKPOINT_INFTY, l_max = BREAKPOINT_INFTY;

		while (pq.GetSize() > 0)
		{
			double* p = (pq.FindMin()).ptr;
			double M = a_left*p[0] - b_left;
			if (M > -W)
			{
				if (a_left > 0)
				{
					// add new breakpoint
					buf[1] = a_left;
					buf[2] = b_left - W;
					buf[0] = buf[2] / buf[1];
					if (l_min < buf[0]) l_min = buf[0];
					Item c; c.ptr = buf; buf += 3;
					pq.Insert( c );
	
					a_left = 0;
					b_left = W;
				}
				break;
			}
			l_min = p[0];
			a_left += p[1];
			b_left += p[2];
			M = a_left*p[0] - b_left;
			if (M > -W)
			{
				p[1] = a_left;
				p[2] = b_left - W;

				a_left = 0;
				b_left = W;

				break;
			}
			pq.DeleteMin();
		}

		while (pq.GetSize() > 0)
		{
			double* p = (pq.FindMax()).ptr;
			double M = a_right*p[0] - b_right;
			if (M < W)
			{
				if (a_right > 0)
				{
					// add new breakpoint
					buf[1] = -a_right;
					buf[2] = -(b_right + W);
					buf[0] = buf[2] / buf[1];
					if (l_max > buf[0]) l_max = buf[0];
					Item c; c.ptr = buf; buf += 3;
					pq.Insert( c );
	
					a_right = 0;
					b_right = -W;
				}
				break;
			}
			l_max = p[0];
			a_right -= p[1];
			b_right -= p[2];
			M = a_right*p[0] - b_right;
			if (M < W)
			{
				p[1] = -a_right;
				p[2] = -(b_right + W);

				a_right = 0;
				b_right = -W;

				break;
			}
			pq.DeleteMax();
		}
		
		if (pq.GetSize() == 0)
		{
			if (a_left > 0)
			{
				buf[1] = a_left;
				buf[2] = b_left - W;
				buf[0] = buf[2] / buf[1];
				if (l_min < buf[0]) l_min = buf[0];
				Item c; c.ptr = buf; buf += 3;
				pq.Insert( c );

				buf[1] = -a_left;
				buf[2] = -(b_left + W);
				buf[0] = buf[2] / buf[1];
				if (l_max > buf[0]) l_max = buf[0];
				c.ptr = buf; buf += 3;
				pq.Insert( c );
	
				a_left = 0;
				b_left = W;
				a_right = 0;
				b_right = -W;
			}
			else
			{
				if      (-b_left < -W) { b_left = b_right = W;  l_min = l_max; }
				else if (-b_left >  W) { b_left = b_right = -W; l_max = l_min; }
			}
		}

//printf("cut at %f, [l_min l_max]=[%f %f]\n", W, l_min, l_max); PrintPQ(pq, a_left, b_left, a_right, b_right);

		if (i == n-1) { solution[i] = l_min; break; }

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

void SolveTVConvexPiecewiseQuadratic(int n, double* unaries, int* breakpoint_num, double* w, double* solution)
{
	_SolveTVConvexPiecewiseQuadratic(n, unaries, breakpoint_num, 0, w, solution);
}

void SolveTVConvexPiecewiseQuadratic(int n, double* unaries, int breakpoint_num, double* w, double* solution)
{
	_SolveTVConvexPiecewiseQuadratic(n, unaries, NULL, breakpoint_num, w, solution);
}
