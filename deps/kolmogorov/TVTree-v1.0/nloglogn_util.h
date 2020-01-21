#ifndef HKUASHGKAJSBGJASHF
#define HKUASHGKAJSBGJASHF

#include <string.h>
#include "block.h"

struct MergeBacklink
{
	double lambda00, lambda01, lambda10, lambda11;

	double GetSolution(double lambda_left, double lambda_right);
};

inline double MergeBacklink::GetSolution(double lambda_left, double lambda_right)
{
	if (lambda_left <= lambda_right)
	{
		if (lambda11 <= lambda_left) return lambda11;
		if (lambda01 <= lambda_left) return lambda_left;
		if (lambda01 <= lambda_right) return lambda01;
		if (lambda00 <= lambda_right) return lambda_right;
		return lambda00;
	}
	else
	{
		if (lambda11 <= lambda_right) return lambda11;
		if (lambda10 <= lambda_right) return lambda_right;
		if (lambda10 <= lambda_left) return lambda10;
		if (lambda00 <= lambda_left) return lambda_left;
		return lambda00;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

struct Backlink
{
	unsigned int p     : 14; // when passing a message, compute and store p\in[0,t]
	unsigned int label : 2; // 0,1 or 2 (the last means copy the right label)
};

struct RestrictedSequence;

// stores mapping T_{ij}:R \times R -> R (as a sequence of mappings \tau_{ij} : R -> R)
struct Sequence
{
	int t;

	double* GetPtr() { return (double*)( (char*)(this) + sizeof(Sequence) ); }
	static int GetMaxSizeInBytes(int t) { return sizeof(Sequence) + (4*t+3)*sizeof(double); }
	static int GetMaxSizeInBytes(int t, int seq_num) { return seq_num*(sizeof(Sequence) + 3*sizeof(double)) + 4*t*sizeof(double); }
	int GetMaxSizeInBytes() { return GetMaxSizeInBytes(t); }

	///// construction
	void Copy(Sequence* s);
	void InitWithUnary(double* unary, int _t, double w_min, double w_max); // enough space should be allocated
	void Merge(Sequence* s2, MergeBacklink* mb, char* buf); // this + s2 ==> this. 'seq' should be of size Sequence::GetMaxSizeInBytes(t+s2->t)

	///// usage
	double PassMessage(double lambda, double M, Backlink& backlink); // updates M, returns backlink\in{-1,0,1}
	Sequence* CutLambdas(double lambda, Backlink backlink, int label);
	void Split(double lambda, Backlink backlink, RestrictedSequence*& left, RestrictedSequence*& right, Buffer& buf); // write result to rs[0] and rs[1]
	double GetMedian();

	///// debugging
	//void Print();
};

// stored mapping R -> R (where the first argument corresponds to lambda)
struct RestrictedSequence
{
	int t;

	double* GetPtr() { return (double*)( (char*)(this) + sizeof(Sequence) ); }
	static int GetMaxSizeInBytes(int t) { return sizeof(Sequence) + (2*t+1)*sizeof(double); }
	int GetMaxSizeInBytes() { return GetMaxSizeInBytes(t); }

	void InitWithUnary(double* unary, int _t);

	///// usage
	double Evaluate(double lambda, int& p);
	int GetBacklink(double lambda, double M, int label);

	RestrictedSequence* CutLambdas(double lambda, int p, int label);

	double GetMedian();
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

inline void Sequence::Copy(Sequence* s)
{
	memcpy(this, s, sizeof(Sequence) + (4*s->t+3)*sizeof(double));
}

inline void Sequence::InitWithUnary(double* unary, int _t, double w_min, double w_max) // enough space should be allocated
{
	t = _t;
	double* ptr = GetPtr();
	*ptr ++ = *unary ++;
	*ptr ++ = w_min;
	*ptr ++ = w_max;
	for ( ; _t>0; _t-- )
	{
		*ptr ++ = *unary ++;
		*ptr ++ = *unary ++;
		*ptr ++ = w_min;
		*ptr ++ = w_max;
	}
}


inline void Sequence::Merge(Sequence* s2, MergeBacklink* mb, char* buf) // this + s2 ==> this. 'seq' should be of size Sequence::GetMaxSizeInBytes(t+s2->t)
{
	double* seq = (double*) buf;

	double* SEQ[2] = { GetPtr(), s2->GetPtr() };
	int T[2] = { t, s2->t };
	t = 0;

	int z00=-1, z01=-1, z10=-1, z11=-1;
	while ( 1 )
	{
#define d SEQ[0][0]
#define a SEQ[0][1]
#define b SEQ[0][2]
#define d_ SEQ[1][0]
#define a_ SEQ[1][1]
#define b_ SEQ[1][2]

		double A = a_ - d;
		double B = b_ - d;

		if (a >= B)
		{
			seq[0] = d_ + b_ - a;
			seq[1] = seq[2] = a;
		}
		else if (b < A)
		{
			seq[0] = d_ + a_ - b;
			seq[1] = seq[2] = b;
			z00 = t;
		}
		else
		{
			seq[0] = d_ + d;

			if (a < A) { seq[1] = A; z10 = t; }
			else       { seq[1] = a; }

			if (b < B) { seq[2] = b; z01 = t; }
			else       { seq[2] = B; }

			z11 = t;
		}
#undef d
#undef a
#undef b
#undef d_
#undef a_
#undef b_

		int k;
		if (T[0] == 0)
		{
			if (T[1] == 0) break;
			k = 1;
		}
		else if (T[1] == 0) k = 0;
		else k = (SEQ[0][3] < SEQ[1][3]) ? 0 : 1;

		seq[3] = SEQ[k][3];
		SEQ[k] += 4;
		T[k] --;
		seq += 4;
		t ++;
	}

	memcpy(GetPtr(), buf, (4*t+3)*sizeof(double));

	double* lambdas = GetPtr() + 3;
	lambdas[4*t] = BREAKPOINT_INFTY;

/*
	if (z01 < z00) z01 = z00;
	if (z10 < z00) z10 = z00;
	if (z11 < z00) z11 = z00;

	mb->lambda00 = (z00>=0) ? lambdas[4*z00] : -BREAKPOINT_INFTY;
	mb->lambda01 = (z01>=0) ? lambdas[4*z01] : -BREAKPOINT_INFTY;
	mb->lambda10 = (z10>=0) ? lambdas[4*z10] : -BREAKPOINT_INFTY;
	mb->lambda11 = (z11>=0) ? lambdas[4*z11] : -BREAKPOINT_INFTY;
*/

	if (z00 < 0)
	{
		mb->lambda00 = -BREAKPOINT_INFTY;
		mb->lambda01 = (z01>=0) ? lambdas[4*z01] : -BREAKPOINT_INFTY;
		mb->lambda10 = (z10>=0) ? lambdas[4*z10] : -BREAKPOINT_INFTY;
		mb->lambda11 = (z11>=0) ? lambdas[4*z11] : -BREAKPOINT_INFTY;
	}
	else
	{
		if (z01 < z00) z01 = z00;
		if (z10 < z00) z10 = z00;
		if (z11 < z00) z11 = z00;

		mb->lambda00 = lambdas[4*z00];
		mb->lambda01 = lambdas[4*z01];
		mb->lambda10 = lambdas[4*z10];
		mb->lambda11 = lambdas[4*z11];
	}
}

inline double Sequence::PassMessage(double lambda, double M, Backlink& backlink)
{
	int p_min = 0, p_max = t;
	double* lambdas = GetPtr()-1; // lambdas[4*(p+1)] gives the p-th breakpoint. Note, lambdas[0] is never evaluated.
	while (p_min < p_max)
	{
		int p = (p_min + p_max + 1) / 2;
		if (lambdas[4*p] < lambda) p_min = p;
		else                       p_max = p-1;
	}
	double* tau = &lambdas[4*p_min+1];
	backlink.p = (unsigned int)p_min;
	if (M < tau[1]) { backlink.label = 1; return tau[1]+tau[0]; }
	if (M < tau[2]) { backlink.label = 2; return M+tau[0]; }
	backlink.label = 0; return tau[2]+tau[0];
}

inline Sequence* Sequence::CutLambdas(double lambda, Backlink backlink, int label)
{
	int p = (int)backlink.p;
	if (label == 0) { t = p; return this; }

	double* lambdas = GetPtr()+3;
	while (p < t && lambdas[4*p] <= lambda) p ++;
	Sequence* s_new = (Sequence*) ( ((double*)this) + 4*p );
	s_new->t = t - p;
	return s_new;
}

inline void Sequence::Split(double lambda, Backlink backlink, RestrictedSequence*& left, RestrictedSequence*& right, Buffer& buf)
{
	int label = (int) backlink.label;
	int p_min = (int)backlink.p, p_max = p_min;
	double* lambdas = GetPtr() + 4*p_max + 3;
	while (p_max < t && lambdas[0] <= lambda) { p_max ++; lambdas += 4; }

	int i;
	double* ptr;

	if (label == 0)
	{
		right = (RestrictedSequence*) buf.Alloc(RestrictedSequence::GetMaxSizeInBytes(t-p_max));
		right->t = t-p_max;
		ptr = right->GetPtr();
		*ptr ++ = lambdas[-1] + lambdas[-3];
		for (i=0; i<right->t; i++)
		{
			*ptr ++ = lambdas[0];
			lambdas += 4;
			*ptr ++ = lambdas[-1] + lambdas[-3];
		}

		ptr = GetPtr();
		lambdas = ptr + 3;
		left = (RestrictedSequence*) this;
		left->t = p_min;
		*ptr ++ = lambdas[-1];
		for (i=0; i<left->t; i++)
		{
			*ptr ++ = lambdas[0];
			lambdas += 4;
			*ptr ++ = lambdas[-1];
		}
	}
	else
	{
		left = (RestrictedSequence*) buf.Alloc(RestrictedSequence::GetMaxSizeInBytes(t-p_max));
		left->t = t-p_max;
		ptr = left->GetPtr();
		*ptr ++ = lambdas[-2];
		for (i=0; i<left->t; i++)
		{
			*ptr ++ = lambdas[0];
			lambdas += 4;
			*ptr ++ = lambdas[-2];
		}

		ptr = GetPtr();
		lambdas = ptr + 3;
		right = (RestrictedSequence*) this;
		right->t = p_min;
		*ptr ++ = lambdas[-2] + lambdas[-3];
		for (i=0; i<right->t; i++)
		{
			*ptr ++ = lambdas[0];
			lambdas += 4;
			*ptr ++ = lambdas[-2] + lambdas[-3];
		}
	}
}

inline double Sequence::GetMedian()
{
	return (GetPtr())[3+4*(t/2)];
}

/*
inline void Sequence::Print()
{
	int _t = t;
	double* ptr = GetPtr();
	for ( ; ; _t-- )
	{
		printf("\t\t%f %f %f\n", ptr[0], ptr[1], ptr[2]); ptr += 3;
		if (_t == 0) break;
		printf("%f\n", *ptr ++);
	}
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

inline void RestrictedSequence::InitWithUnary(double* unary, int _t) // enough space should be allocated
{
	t = _t;
	memcpy(GetPtr(), unary, (2*t+1)*sizeof(double));
}

inline double RestrictedSequence::GetMedian()
{
	return (GetPtr())[1 + 2*(t/2)];
}

inline double RestrictedSequence::Evaluate(double lambda, int& _p)
{
	int p_min = 0, p_max = t;
	double* lambdas = GetPtr()-1; // lambdas[2*(p+1)] gives the p-th breakpoint. Note, lambdas[0] is never evaluated.
	while (p_min < p_max)
	{
		int p = (p_min + p_max + 1) / 2;
		if (lambdas[2*p] < lambda) p_min = p;
		else                       p_max = p-1;
	}
	_p = p_min;
	return lambdas[2*p_min+1];
}

inline RestrictedSequence* RestrictedSequence::CutLambdas(double lambda, int p, int label)
{
	if (label == 0) { t = p; return this; }

	double* lambdas = GetPtr()+1;
	while (p < t && lambdas[2*p] <= lambda) p ++;
	RestrictedSequence* s_new = (RestrictedSequence*) ( ((double*)this) + 2*p );
	s_new->t = t - p;
	return s_new;
}

#endif