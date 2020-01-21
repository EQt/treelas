/* Copyright 2014 Vladimir Kolmogorov vnk@ist.ac.at */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TVchain.h"
#include "nloglogn_util.h"
#include "block.h"

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

Sequence* Contract(int n, double*& unaries, int* unaries_breakpoint_num, double* w, MergeBacklink* mb_array, ReusableBuffer& _buf)
{
	int i, level, level_max;
	for (level_max=0; (1<<level_max) < n; level_max ++) {}

	char* buf = (char*)_buf.Alloc((n/2+1)*sizeof(double));
	int buf_size = 0;

	int* stack = new int[level_max+1]; // k-th sequence in the stack occupies buf[stack[k]..stack[k+1]]
	stack[0] = 0;
	int* stack_current = stack;

	i = -1; level = 0;
	while ( 1 )
	{
		if (i < 0 || ((i >> level) & 1) == 0)	// if left child
		{
			int ii = i + (1 << level);
			if (ii >= n-1)
			{
				if (level >= level_max) break;
				level ++;
				continue;
			}
			i = ii;
			level = 0;

			// add edge (i,i+1) to stack
			int t = *unaries_breakpoint_num ++;
			stack_current[0] = buf_size;
			buf_size += Sequence::GetMaxSizeInBytes(t);
			buf = (char*) _buf.Realloc(buf_size);
			Sequence* s = (Sequence*) &buf[stack_current[0]];
			double W = w[i];
			s->InitWithUnary(unaries, t, -W, W);
			unaries += 2*t+1;
			stack_current ++;
		}
		else
		{
			i -= 1 << level;
			level ++;
			// merge the last two elements of the stack
			stack_current --;

			Sequence* s = (Sequence*) &buf[stack_current[-1]];
			Sequence* s2 = (Sequence*) &buf[stack_current[0]];
			buf = (char*) _buf.Realloc(buf_size + Sequence::GetMaxSizeInBytes(s->t + s2->t));
			s = (Sequence*) &buf[stack_current[-1]];
			s2 = (Sequence*) &buf[stack_current[0]];
			s->Merge(s2, mb_array ++, buf + buf_size);
			buf_size = stack_current[-1] + Sequence::GetMaxSizeInBytes(s->t);
		}
	}

	delete [] stack;

	return (Sequence*) buf;
}

struct ContractedSubproblem
{
	int start;
	double lambda_min;
	bool lambda_max_is_infty;
	RestrictedSequence* rs_end;
};

struct IntDouble { int t; double lambda; };

inline int RandomInteger(int N) // returns random integer in [0,N-1]
{
	while ( 1 )
	{
		int x = (int) ( (((double)N) * rand()) / RAND_MAX );
		if (x<N) return x;
	}
}

bool ChoosePivot(Sequence** seq_array, int len, RestrictedSequence* rs_end, double& lambda, ReusableBuffer* rbuf)
{
	int i, n=0, w_sum=0;
	IntDouble* arr = (IntDouble*) rbuf->Alloc((len+1)*sizeof(IntDouble));

	RestrictedSequence* rs_start = (RestrictedSequence*) seq_array[0];
	if (rs_start->t > 0) { arr[n].t = rs_start->t; arr[n++].lambda = rs_start->GetMedian(); }
	if (rs_end->t > 0) { arr[n].t = rs_end->t; arr[n++].lambda = rs_end->GetMedian(); }

	for (seq_array++; len>1; seq_array++, len--)
	{
		int t = seq_array[0]->t;
		if (t > 0) { arr[n].t = t; w_sum += t; arr[n++].lambda = seq_array[0]->GetMedian(); }
	}

	if (n == 0) return false;

//	lambda = arr[n/2].lambda;

	w_sum /= 2;
	while (n > 1)
	{
		double pivot = arr[RandomInteger(n)].lambda;
		int w_less = 0, w_equal = 0;
		for (i=0; i<n; i++)
		{
			if      (arr[i].lambda <  pivot) w_less  += arr[i].t;
			else if (arr[i].lambda == pivot) w_equal += arr[i].t;
		}
		if (w_less+w_equal > w_sum) // lambda^* <= pivot
		{
			if (w_less <= w_sum) { arr[0].lambda = pivot; break; }
			int n_old = n;
			n = 0;
			for (i=0; i<n_old; i++) { if (arr[i].lambda < pivot) arr[n++] = arr[i]; }
		}
		else
		{
			w_sum -= w_less + w_equal;
			int n_old = n;
			n = 0;
			for (i=0; i<n_old; i++) { if (arr[i].lambda > pivot) arr[n++] = arr[i]; }
		}
	}

	lambda = arr[0].lambda;
	return true;
}

// seq_array and x: of size N
void SolveContracted(int N, Sequence** seq_array, double* solution, ReusableBuffer* rbuf)
{
	int i, j;
	Buffer buf(N);
	ContractedSubproblem* stack = new ContractedSubproblem[N+1];

	ContractedSubproblem* s = stack+1;
	stack->start = N;
	s->start = 0;
	s->lambda_min = -BREAKPOINT_INFTY;
	s->lambda_max_is_infty = true;
	s->rs_end = (RestrictedSequence*) buf.Alloc(RestrictedSequence::GetMaxSizeInBytes(0));
	double tmp_double = 0;
	s->rs_end->InitWithUnary(&tmp_double, 0);

	while ( s > stack )
	{
		ContractedSubproblem s_current = s[0];
		s --;

		double lambda;
		int len = s->start - s_current.start;
		if (!ChoosePivot(seq_array+s_current.start, len, s_current.rs_end, lambda, rbuf))
		{
			if (!s_current.lambda_max_is_infty)
			{
				for (i=s_current.start; i<s->start; i++) solution[i] = s_current.lambda_min;
				continue;
			}
			lambda = BREAKPOINT_INFTY;
			s_current.lambda_max_is_infty = false;
		}

		// solve subproblem for indices [s_current.start, s->start-1] \subseteq [0,N-1]
		Backlink* backlinks = ((Backlink*) rbuf->Alloc(len*sizeof(Backlink))) - (s_current.start+1);
		int p_start, p_end;
		
		double M = ((RestrictedSequence*)seq_array[s_current.start])->Evaluate(lambda, p_start);
		for (i=s_current.start+1; i<s->start; i++)
		{
			M = seq_array[i]->PassMessage(lambda, M, backlinks[i]);
		}
		i --;
		int label = (M < s_current.rs_end->Evaluate(lambda, p_end)) ? 1 : 0;
		s_current.rs_end = s_current.rs_end->CutLambdas(lambda, p_end, label);
		for (i--; i>=s_current.start; i--)
		{
			int label_prev = label;
			label = (backlinks[i+1].label > 1) ? label : backlinks[i+1].label;
			if (label_prev == label) continue;
			s ++;
			s->start = i+1;
			if (label_prev == 0) { s->lambda_min = s_current.lambda_min; s->lambda_max_is_infty = false; }
			else                 { s->lambda_min = lambda; s->lambda_max_is_infty = s_current.lambda_max_is_infty; }

			s->rs_end = s_current.rs_end;

			seq_array[s->start]->Split(lambda, backlinks[s->start], s_current.rs_end, *((RestrictedSequence**)&seq_array[s->start]), buf);

			for (j=s->start+1; j<(s-1)->start; j++)
			{
				seq_array[j] = seq_array[j]->CutLambdas(lambda, backlinks[j], label_prev);
			}
		}
		s ++;
		s->start = i+1;
		if (label == 0) { s->lambda_min = s_current.lambda_min; s->lambda_max_is_infty = false; }
		else            { s->lambda_min = lambda; s->lambda_max_is_infty = s_current.lambda_max_is_infty; }

		s->rs_end = s_current.rs_end;

		seq_array[s->start] = (Sequence*) (((RestrictedSequence*)seq_array[s->start])->CutLambdas(lambda, p_start, label));

		for (j=s->start+1; j<(s-1)->start; j++)
		{
			seq_array[j] = seq_array[j]->CutLambdas(lambda, backlinks[j], label);
		}
	}

	delete [] stack;
}

void FillGaps(int n, MergeBacklink* mb_array, double* solution)
{
	if (n <= 2) return;

	int i, level;

	mb_array += n-3;

	i=0; level=1;
	while ((1 << level) < n-1) level ++;

	while ( 1 )
	{
		int shift = 1 << (level-1);
		int j = i + shift;
		int k = j + shift;
		if (j < n-1)
		{
			if (k > n-1) k = n-1;
			// infer solution[j] from (solution[i], solution[k])

			solution[j] = mb_array->GetSolution(solution[i], solution[k]);
			mb_array --;
		}

		// move to next
		if (level > 1) { i = j; level --; } // right_child
		else
		{
			if (i == 0) break;
			// upright ... upright upleft left_child
			while ((i & (1 << level)) == 0) level ++;
			i -= 1 << level;
		}
	}
}

void _SolveTVConvexPiecewiseLinear_nloglogn(int n, double* unaries, int* breakpoint_num_array, int breakpoint_num, double* w, double* solution)
{
	int i, k, total_breakpoint_num=0;
	if (breakpoint_num_array)
	{
		for (i=0; i<n; i++) total_breakpoint_num += breakpoint_num_array[i];
	}
	else total_breakpoint_num = n*breakpoint_num;

	int T = 1;
	while ((1 << T) <= n) T *= 2;
	T ++;  // make T=2^c+1 to get balanced binary trees

	int N = (n-2) / T + 2;
	if (N > n) N = n;

	ReusableBuffer rbuf;
	int contract_data_buf_size =
			  N*sizeof(Sequence*)
			+ (n-N)*sizeof(MergeBacklink)
			+ N*sizeof(double)
			+ (breakpoint_num_array ? 0 : (T*sizeof(int)))
			+ Sequence::GetMaxSizeInBytes(total_breakpoint_num, N);
	char* contract_data_buf = new char[contract_data_buf_size];
	char* ptr_buf = contract_data_buf;

	Sequence** seq_array = (Sequence**) ptr_buf; ptr_buf = (char*) (seq_array + N);
	MergeBacklink* mb_array = (MergeBacklink*) ptr_buf; ptr_buf = (char*) (mb_array + n-N);
	double* solution_subsampled = (double*) ptr_buf; ptr_buf = (char*) (solution_subsampled + N);
	int* _breakpoint_num_array;
	if (!breakpoint_num_array)
	{
		_breakpoint_num_array = (int*) ptr_buf; ptr_buf = (char*) (_breakpoint_num_array + T);
		for (i=0; i<T; i++) _breakpoint_num_array[i] = breakpoint_num;
	}

	double* unaries0 = unaries;
	MergeBacklink* mb_array0 = mb_array;
	seq_array[0] = (Sequence*) ptr_buf;
	int t = (breakpoint_num_array) ? breakpoint_num_array[0] : breakpoint_num;
	((RestrictedSequence*)seq_array[0])->InitWithUnary(unaries, t);
	unaries += 2*t+1;
	ptr_buf += ((RestrictedSequence*)seq_array[0])->GetMaxSizeInBytes();

	if (n > 1)
	for (i=0, k=1; ; i+=T, k++)
	{
		int len = (n-i-1 <= T) ? n-i-1 : T;
		Sequence* s = Contract(len+1, unaries, breakpoint_num_array ? breakpoint_num_array+i+1 : _breakpoint_num_array, w+i, mb_array, rbuf);
		seq_array[k] = (Sequence*) ptr_buf;
		seq_array[k]->Copy(s);
		mb_array += T-1;
		ptr_buf += seq_array[k]->GetMaxSizeInBytes();
		if (n-i-1 <= T) break;
	}

	SolveContracted(N, seq_array, solution_subsampled, &rbuf);
	solution[0] = *solution_subsampled ++;

	if (n > 1)
	for (i=0, k=1; ; i+=T, k++)
	{
		int len = (n-i-1 <= T) ? n-i-1 : T;
		solution[len] = *solution_subsampled ++;
		FillGaps(len+1, mb_array0, solution);
		mb_array0 += T-1;
		solution += T;
		if (n-i-1 <= T) break;
	}

	delete [] contract_data_buf;
}


void SolveTVConvexPiecewiseLinear_nloglogn(int n, double* unaries, int* breakpoint_num_array, double* w, double* solution)
{
	_SolveTVConvexPiecewiseLinear_nloglogn(n, unaries, breakpoint_num_array, 0, w, solution);
}

void SolveTVConvexPiecewiseLinear_nloglogn(int n, double* unaries, int breakpoint_num, double* w, double* solution)
{
	_SolveTVConvexPiecewiseLinear_nloglogn(n, unaries, NULL, breakpoint_num, w, solution);
}
