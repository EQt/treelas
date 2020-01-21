/* Copyright 2014 Vladimir Kolmogorov vnk@ist.ac.at */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TVchain.h"
#include "block.h"

struct Message
{
	Message();
	~Message();

	void AddUnary(double* unary, int t); // t = # breakpoints in 'unary'

	//void DTRight(double w);
	void DTRightRev(double w); // do distance transform corresponding to pairwise term f_{ij}(z)=-\infty for z<0 and f_{ij}(z)=wz for z>=0
	                           // write result in the opposite order (and with negative signs)
	                           // applying this function twice will correspond to DT with f_{ij}(z)=w|z|
	void DTTruncate(double c);

	double* ResetBacklinks(int& num);

	double ComputeMin();

	void Print() { int i; for (i=0; i<2*t+1; i++) printf("%f ", M[i]); printf("\n"); }

	////////////////////////////////////////////
	double* buf;
	int buf_size;
	double* M;
	int t; // # breapoints in M. The length of M is (2*t+1)

	////////////////////////////////////////////
	ReusableBuffer buf_backlinks;
	double* backlinks;
	int backlinks_num;
	void AddBacklink(double lambda);
	void AddBacklink(double lambda1, double lambda2);
};

Message::Message()
{
	buf_size = 16;
	buf = new double[2*buf_size];
	M = buf;
	t = 0;
	M[0] = 0;

	backlinks = (double*) buf_backlinks.Alloc(16*sizeof(double));
	backlinks_num = 0;
}

Message::~Message()
{
	delete [] buf;
}

double* Message::ResetBacklinks(int& num)
{
	num = backlinks_num;
	backlinks_num = 0;
	return backlinks;
}

inline void Message::AddBacklink(double lambda)
{
	backlinks = (double*) buf_backlinks.Realloc((backlinks_num+1)*sizeof(double));
	backlinks[backlinks_num ++] = lambda;
}

inline void Message::AddBacklink(double lambda1, double lambda2)
{
	backlinks = (double*) buf_backlinks.Realloc((backlinks_num+2)*sizeof(double));
	backlinks[backlinks_num ++] = lambda1;
	backlinks[backlinks_num ++] = lambda2;
}

double Message::ComputeMin()
{
	if (M[0] > 0) return -BREAKPOINT_INFTY;
	if (M[2*t] < 0) return BREAKPOINT_INFTY;
	if (t == 0) return 0;

	double* ptr = M + 1;
	double* ptr_end = M + 2*t - 1;
	double lambda = *ptr;
	double y_lambda = 0;
	double y = 0;
	for ( ; ptr < ptr_end; ptr += 2 )
	{
		y += (ptr[2] - ptr[0]) * ptr[1];
		if (y_lambda > y)
		{
			y_lambda = y;
			lambda = ptr[2];
		}
	}
	return lambda;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Message::AddUnary(double* ptr1, int t1)
{
	double* ptr0 = M;
	double* buf_old = NULL;

	if (2*(t+t1)+1 > buf_size)
	{
		buf_size += buf_size / 2 + 2*t1;
		buf_old = buf;
		buf = new double[2*buf_size];
		M = buf;
	}

	double* M_new = M = (M < buf + buf_size) ? (buf + buf_size) : buf;

	int t0 = t;
	double s0 = *ptr0 ++;
	double s1 = *ptr1 ++;
	*M_new ++ = s0 + s1;
	for (t=0; ; t++)
	{
		if (t1 == 0)
		{
			for ( ; t0>0; t0--, t++) { *M_new ++ = *ptr0 ++; *M_new ++ = (*ptr0 ++) + s1; }
			break;
		}
		if (t0 == 0)
		{
			for ( ; t1>0; t1--, t++) { *M_new ++ = *ptr1 ++; *M_new ++ = (*ptr1 ++) + s0; }
			break;
		}
		if ((*ptr0) < (*ptr1)) { *M_new ++ = *ptr0 ++; s0 = *ptr0 ++; t0--; }
		else                   { *M_new ++ = *ptr1 ++; s1 = *ptr1 ++; t1--; }
		*M_new ++ = s0 + s1;
	}

	if (buf_old) delete [] buf_old;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
void Message::DTRight(double w)
{
	if (M[0] > w)
	{
		M[0] = w;
		t = 0;
		AddBacklink(-BREAKPOINT_INFTY, BREAKPOINT_INFTY);
		return;
	}
	if (t == 0)
	{
		return;
	}

	double* ptr = M;
	double* ptr_end = M + 2*t;
	double* ptr_new = M = (M < buf + buf_size) ? (buf + buf_size) : buf;
	*ptr_new ++ = *ptr ++;
	*ptr_new ++ = *ptr ++;
	t = 1;

	while ( 1 )
	{
		if (ptr == ptr_end)
		{
			*ptr_new = ((*ptr) < w) ? (*ptr) : w;
			return;
		}

		if ((*ptr) <= w)
		{
			*ptr_new ++ = *ptr ++;
			*ptr_new ++ = *ptr ++;
			t ++;
			continue;
		}

		double y = 0;
		*ptr_new ++ = w;
		bool flag = false;
		while ( 1 )
		{
			double s = *ptr;
			if (ptr == ptr_end)
			{
				if (s < w)
				{
					*ptr_new ++ = ptr[-1] + y / (w - s);
					*ptr_new ++ = s;
					t ++;
					AddBacklink(ptr_new[-4], ptr_new[-2]);
				}
				else
				{
					AddBacklink(ptr_new[-1], BREAKPOINT_INFTY);
				}
				return;
			}
			double y_old = y;
			y += (ptr[1] - ptr[-1])*(s - w);
			if (y < 0 && flag)
			{
				*ptr_new ++ = ptr[-1] + y_old / (w - s);
				if ((*ptr_new) > ptr[1]) (*ptr_new) = ptr[1]; // shouldn't happen, but just in case
				*ptr_new ++ = s;
				AddBacklink(ptr_new[-4], ptr_new[-2]);
				ptr ++;
				*ptr_new ++ = *ptr ++;
				t += 2;
				break;
			}
			ptr += 2;
			flag = true;
		}
	}
}
*/
void Message::DTRightRev(double w)
{
	if (M[0] > w)
	{
		M[0] = -w;
		t = 0;
		AddBacklink(-BREAKPOINT_INFTY, BREAKPOINT_INFTY);
		return;
	}
	if (t == 0)
	{
		M[0] = -M[0];
		return;
	}

	double* ptr = M;
	double* ptr_end = M + 2*t;
	M = (M < buf + buf_size) ? (buf + buf_size) : buf;
	M += 2*t;
	*M -- = - (*ptr ++);
	*M -- = - (*ptr ++);
	t = 1;

	while ( 1 )
	{
		if (ptr == ptr_end)
		{
			if ((*ptr) <= w) *M = - (*ptr);
			else
			{
				*M = - w;
				AddBacklink(-M[1], BREAKPOINT_INFTY);
			}
			return;
		}

		if ((*ptr) <= w)
		{
			*M -- = - (*ptr ++);
			*M -- = - (*ptr ++);
			t ++;
			continue;
		}

		double y = 0;
		*M -- = -w;
		bool flag = false;
		while ( 1 )
		{
			double s = *ptr;
			if (ptr == ptr_end)
			{
				if (s < w)
				{
					*M -- = - ( ptr[-1] + y / (w - s) );
					*M    = -s;
					t ++;
					AddBacklink(-M[3], -M[1]);
				}
				else
				{
					M ++;
					AddBacklink(-M[1], BREAKPOINT_INFTY);
				}
				return;
			}
			double y_old = y;
			y += (ptr[1] - ptr[-1])*(s - w);
			if (y < 0 && flag)
			{
				*M -- = - ( ptr[-1] + y_old / (w - s) );
				if ((*M) < -ptr[1]) (*M) = -ptr[1]; // shouldn't happen, but just in case
				*M -- = -s;
				AddBacklink(-M[4], -M[2]);
				ptr ++;
				*M -- = - (*ptr ++);
				t += 2;
				break;
			}
			ptr += 2;
			flag = true;
		}
	}
}

void Message::DTTruncate(double c)
{
	if (M[0] > 0 || M[2*t] < 0)
	{
		M[0] = 0;
		t = 0;
		AddBacklink((M[0] > 0) ? BREAKPOINT_INFTY : -BREAKPOINT_INFTY);
		AddBacklink(-BREAKPOINT_INFTY, BREAKPOINT_INFTY);
		return;
	}
	if (t == 0)
	{
		AddBacklink(0);
		return;
	}

	// first pass: determine min of M
	double y = 0;
	double* ptr = M + 1;
	double* ptr_end = M + 2*t-1;
	double lambda_best = ptr[0];
	double y_best = 0;
	for ( ; ptr < ptr_end; ptr += 2)
	{
		y += (ptr[2] - ptr[0]) * ptr[1];
		if (y_best > y) { y_best = y; lambda_best = ptr[2]; }
	}
	y_best += c;
	AddBacklink(lambda_best);

	// prepare space for the new message
	ptr = M + 1;
	double* buf_old = NULL;

	if (2*(2*t+1)+1 > buf_size)
	{
		buf_size += buf_size / 2 + 2*(2*t+1)+1;
		buf_old = buf;
		buf = new double[2*buf_size];
		M = buf;
	}
	else M = (M < buf + buf_size) ? (buf + buf_size) : buf;

	// second pass
	double* ptr_new = M;
	*ptr_new ++ = 0;
	t = 0;
	y = -y_best;
	for ( ; ; ptr += 2 )
	{
		if (y < 0) break;
		if (ptr == ptr_end)
		{
			AddBacklink(-BREAKPOINT_INFTY, BREAKPOINT_INFTY);
			if (buf_old) delete [] buf_old;
			return;
		}
		y += (ptr[2] - ptr[0]) * ptr[1];
	}

	if (ptr[-1] < 0)
	{
		*ptr_new ++ = ptr[0] - y / ptr[-1];
		AddBacklink(-BREAKPOINT_INFTY, ptr_new[-1]);
		*ptr_new ++ = ptr[-1];
		t ++;
	}

	while ( 1 )
	{
		// y < 0
		*ptr_new ++ = *ptr ++;
		*ptr_new ++ = *ptr ++;
		t ++;

		if (ptr > ptr_end)
		{
			if (ptr[-1] > 0)
			{
				*ptr_new ++ = ptr[-2] - y / ptr[-1];
				*ptr_new = 0;
				t ++;
				AddBacklink(ptr_new[-1], BREAKPOINT_INFTY);
			}
			if (buf_old) delete [] buf_old;
			return;
		}

		y += (ptr[0] - ptr[-2]) * ptr[-1];
		if (y < 0) continue;

		double lambda1 = (ptr[-1] > 0) ? (ptr[0] - y / ptr[-1]) : ptr[0];
		*ptr_new ++ = lambda1;
		*ptr_new ++ = 0;
		t ++;

		while ( 1 )
		{
			if (ptr == ptr_end)
			{
				if (ptr[1] < 0)
				{
					*ptr_new ++ = ptr[0] - y / ptr[1];
					*ptr_new ++ = ptr[1];
					t ++;
					AddBacklink(lambda1, ptr_new[-2]);
				}
				else AddBacklink(lambda1, BREAKPOINT_INFTY);
				if (buf_old) delete [] buf_old;
				return;
			}

			y += (ptr[2] - ptr[0]) * ptr[1];
			ptr += 2;
			if (y < 0)
			{
				*ptr_new ++ = (ptr[-1] < 0) ? (ptr[0] - y / ptr[-1]) : ptr[0];
				*ptr_new ++ = ptr[-1];
				t ++;
				AddBacklink(lambda1, ptr_new[-2]);
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// exactly one of breakpoint_num_array, breakpoint_num must be 0
void _SolveTVPiecewiseLinear(int n, double* unaries, int* breakpoint_num_array, int breakpoint_num, double* w, double* c, double* solution)
{
	Message M;
	int i;

	Buffer backlinks_buf(n);

	double** backlinks[3];
	int* backlinks_num[3];

	backlinks[0] = new double*[2*(n-1) + ((c==NULL) ? 0 : (n-1))];
	backlinks_num[0] = new int[2*(n-1) + ((c==NULL) ? 0 : (n-1))];
	backlinks[1] = backlinks[0] + n-1;
	backlinks_num[1] = backlinks_num[0] + n-1;
	backlinks[2] = backlinks[1] + n-1;
	backlinks_num[2] = backlinks_num[1] + n-1;

	double* tmp;
	int tmp_num;

	for (i=0; ; i++)
	{
		int t = breakpoint_num_array ? breakpoint_num_array[i] : breakpoint_num;
		M.AddUnary(unaries, t);
		unaries += 2*t+1;

		if (i == n-1) break;

		int dir;
		for (dir=0; dir<3; dir++)
		{
			if (dir < 2)
			{
				M.DTRightRev(w[i]);
			}
			else
			{
				if (c == NULL) break;
				M.DTTruncate(c[i]);
			}
			tmp = M.ResetBacklinks(tmp_num);
			backlinks[dir][i] = (double*) backlinks_buf.Alloc(tmp_num*sizeof(double));
			memcpy(backlinks[dir][i], tmp, tmp_num*sizeof(double));
			backlinks_num[dir][i] = tmp_num;
		}
	}

	double lambda = solution[i] = M.ComputeMin();

	for (i-- ; i>=0; i--)
	{
		int k;
		bool stop_flag = false;
		if (c != NULL)
		{
			for (k=1; k<backlinks_num[2][i]; k+=2)
			{
				if (backlinks[2][i][k] <= lambda && lambda <= backlinks[2][i][k+1])
				{
					solution[i] = lambda = backlinks[2][i][0];
					stop_flag = true;
					break;
				}
			}
			if (stop_flag) continue;
		}

		for (k=0; k<backlinks_num[1][i]; k+=2)
		{
			if (-backlinks[1][i][k+1] <= lambda && lambda <= -backlinks[1][i][k])
			{
				solution[i] = lambda = -backlinks[1][i][k];
				stop_flag = true;
				break;
			}
		}
		if (stop_flag) continue;

		for (k=0; k<backlinks_num[0][i]; k+=2)
		{
			if (backlinks[0][i][k] <= lambda && lambda <= backlinks[0][i][k+1])
			{
				lambda = backlinks[0][i][k];
				break;
			}
		}
		solution[i] = lambda;
	}

	delete [] backlinks[0];
	delete [] backlinks_num[0];
}



void SolveTVPiecewiseLinear(int n, double* unaries, int* breakpoint_num, double* w, double* c, double* solution)
{
	_SolveTVPiecewiseLinear(n, unaries, breakpoint_num, 0, w, c, solution);
}

void SolveTVPiecewiseLinear(int n, double* unaries, int breakpoint_num, double* w, double* c, double* solution)
{
	_SolveTVPiecewiseLinear(n, unaries, NULL, breakpoint_num, w, c, solution);
}

