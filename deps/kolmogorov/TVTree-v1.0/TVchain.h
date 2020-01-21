/* Copyright 2014-2015 Vladimir Kolmogorov vnk@ist.ac.at */

#ifndef GHAUDFHAUSGHAPONIHJBQFIJA
#define GHAUDFHAUSGHAPONIHJBQFIJA


// minimize function of the form
//
//    f(x_0,...,x_{n-1}) = \sum_{i=0,..,n-1} f_i(x_i) + \sum_{(i,j):i=0,...,n-2,j=i+1} f_{ij}(x_j - x_i)
//
// Cases:
//    1. unary terms f_i(z):        convex quadratic
//       pairwise terms f_{ij}(z):  = w_{ij} |z|
//
//    2. unary terms f_i(z):        convex piecewise-linear
//       pairwise terms f_{ij}(z):  = w_{ij} |z|
//
//    3. unary terms f_i(z):        arbitrary piecewise-linear
//       pairwise terms f_{ij}(z):  = \min [ w_{ij} |z| , c_{ij} ]     where c_{ij}>=0 (can be infinities)
//
//    4. unary terms f_i(z):        convex piecewise-quadratic
//       pairwise terms f_{ij}(z):  = w_{ij} |z|
//
// 2 is a special case of 3, but can be solved more efficiently (namely, in O(n log n) time)




// Worst-case complexities (assuming that the number of breakpoints in unary terms is O(1)):
// 1. SolveTVConvexQuadratic()                  O(n)
// 2. SolveTVConvexPiecewiseLinear()            O(n \log n)
//    SolveTVConvexPiecewiseLinear_nloglogn()   O(n \log \log n)
// 3. SolveTVPiecewiseLinear()                  O(n*n) for non-truncated TV (i.e. if c_{ij}==NULL)
//                                              non-polynomial for truncated TV
// 4. SolveTVConvexPiecewiseQuadratic()         O(n \log n)
//
// If the "interaction radius" is bounded by some constant D (i.e. node i has no effect on node i+D)
// then algorithms should be linear time.
//
// O(n \log n) algorithms in (2) and (4) use MinMaxHeap
// (M. D. Atkinson, J.R. Sack, N. Santoro, and T. Strothotte 1986)



#define BREAKPOINT_INFTY (10000) // if optimal solution x[i] is unbounded then x[i] will be set
                                 // either to -BREAKPOINT_INFTY or to BREAKPOINT_INFTY.
                                 // Can be numerically unstable. It is thus recommended to
                                 // apply the code only to problems in which ALL optimal solutions are bounded.





///////////////////////////////////////////////////////////////////////////////////////////
//                    1. Convex quadratic unary terms + TV regularizer                   //
///////////////////////////////////////////////////////////////////////////////////////////

//       unary terms f_i(z):        = a_i/2 z*z - b_i z     with a_i>0
//       pairwise terms f_{ij}(z):  = w_{ij} |z|

// Input: a, b: arrays of size n
//        w: array of size n-1
// Output: solution (this array of size n must be allocated by the user)
//
// Complexity: O(n)
void SolveTVConvexQuadratic(int n, double* a, double* b, double* w, double* solution);

// same as above but assumes that all coefficients a_i equal 1
void SolveTVConvexQuadratic_a1(int n, double* b, double* w, double* solution);






///////////////////////////////////////////////////////////////////////////////////////////
//                2. Convex piecewise-linear unary terms + TV regularizer                //
///////////////////////////////////////////////////////////////////////////////////////////

//       unary terms f_i(z):        convex piecewise-linear
//       pairwise terms f_{ij}(z):  = w_{ij} |z|

//       a piecewise-linear function with t breakpoints is represented by a sequence
//          s_0, lambda_1, ..., lambda_t, s_t
//       of size 2*t+1, with lambda_1 <= ... <= lambda_t
//       lambda_k is the X-coordinate of the k-th breakpoint, and s_k is the slope of the k-th segment.
//       E.g. term f_i(x_i) = 2*|x_i-1| would be represented as   -2, 1, 2.

// Input: breakpoint_num_array : array of size n
//        unaries: array of size \sum_{i=0}^{n-1}  (2*breakpoint_num_array[i] + 1)
//                 (contains sequences for terms f_0,...,f_{n-1} placed consecutively in a single array).
//
//        For each unary term must have s_0 <= ... s_t (since the unaries are convex functions).
//
//        w: array of size n-1
// Output: solution (this array of size n must be allocated by the user)
// IMPORTANT: the code modifies slopes in 'unaries'!!! (lambda's are unchanged)
void SolveTVConvexPiecewiseLinear(int n, double* unaries, int* breakpoint_num_array, double* w, double* solution);

// same as above, but the number of breakpoints is the same for all unary terms.
void SolveTVConvexPiecewiseLinear(int n, double* unaries, int breakpoint_num, double* w, double* solution);

// Alternative algorithm with complexity O(n \log \log n). (This is better than O(n \log n) for the function above,
// but in practice should be slower, unless the regularization is very strong)
void SolveTVConvexPiecewiseLinear_nloglogn(int n, double* unaries, int* breakpoint_num_array, double* w, double* solution);
void SolveTVConvexPiecewiseLinear_nloglogn(int n, double* unaries, int breakpoint_num, double* w, double* solution);



///////////////////////////////////////////////////////////////////////////////////////////
//      3. Arbitrary piecewise-linear unary terms + [truncated] TV regularizer           //
///////////////////////////////////////////////////////////////////////////////////////////

//       unary terms f_i(z):        = arbitrary piecewise-linear
//       pairwise terms f_{ij}(z):  = \min [ w_{ij} |z| , c_{ij} ]

// Input: unaries, breakpoint_num_array: arrays describing unaries, as described above in Case 2.
//        The slopes s_0, ..., s_t may now be arbitrary (not necessarily non-decreasing).
//        w, c: arrays of size n-1 (if c == NULL then there is no truncation)
// Output: solution (this array of size n must be allocated by the user)
//
// Complexity: O(n*n) if c_{ij}==NULL, otherwise non-polynomial
void SolveTVPiecewiseLinear(int n, double* unaries, int* breakpoint_num_array, double* w, double* c, double* solution);

// same as above, but the number of breakpoints is the same for all unary terms.
void SolveTVPiecewiseLinear(int n, double* unaries, int breakpoint_num, double* w, double* c, double* solution);








///////////////////////////////////////////////////////////////////////////////
//      4. Convex piecewise-quadratic unary terms + TV regularizer           //
///////////////////////////////////////////////////////////////////////////////

//       unary terms f_i(z):        = convex piecewise-quadratic
//       pairwise terms f_{ij}(z):  = \min [ w_{ij} |z| , c_{ij} ]

//       a piecewise-quadratic function with t breakpoints is represented by a sequence
//          a_0, b_0, lambda_1, a_1, b_1, lambda_2, ..., lambda_t, a_t, b_t
//       of size 3*t+2, with lambda_1 <= ... <= lambda_t
//       lambda_k is the X-coordinate of the k-th breakpoint, and (a_k, b_k) define function h(z)=a_k/2 z*z - b_k z.
//       Convexity of f_i means that coefficients a_i are non-negative and for each breakpoint the left derivative
//       is smaller or equal than the right derivative.

// Input: breakpoint_num_array : array of size n
//        unaries: array of size \sum_{i=0}^{n-1}  (3*breakpoint_num[i] + 2)
//                 (contains sequences for terms f_0,...,f_{n-1} placed consecutively in a single array).
//        w: array of size n-1
// Output: solution (this array of size n must be allocated by the user)
// IMPORTANT: the code modifies coefficients a,b in 'unaries'!!! (lambda's are unchanged)
void SolveTVConvexPiecewiseQuadratic(int n, double* unaries, int* breakpoint_num_array, double* w, double* solution);

// same as above, but the number of breakpoints is the same for all unary terms.
void SolveTVConvexPiecewiseQuadratic(int n, double* unaries, int breakpoint_num, double* w, double* solution);













////////// for debugging

// returns f(lambda) where 'f' is the piecewise-linear function represented by 'unary'
// assumes that f(first breapoint)=0  (or f(0)=0 if breakpoint_num==0)
inline double GetUnary(double* unary, int breakpoint_num, double lambda)
{
	if (breakpoint_num == 0) return unary[0] * lambda;
	if (lambda <= unary[1]) return (lambda - unary[1])*unary[0];
	double y = 0;
	for (unary+=2; breakpoint_num>1; unary+=2, breakpoint_num--)
	{
		if (lambda <= unary[1]) break; 
		y += (unary[1] - unary[-1])*unary[0];
	}
	return y + (lambda - unary[-1])*unary[0];
}

// returns the cost of labeling 'solution'
inline double EvaluateTVPiecewiseLinear(int n, double* unaries, int* unaries_breakpoint_num, double* w, double* c, double* solution)
{
	int i;
	double cost = 0;
	for (i=0; i<n; i++)
	{
		cost += GetUnary(unaries, unaries_breakpoint_num[i], solution[i]);
		unaries += 2*unaries_breakpoint_num[i]+1;
	}
	for (i=0; i<n-1; i++)
	{
		double diff = solution[i+1] - solution[i];
		double q = (diff >= 0) ? w[i]*diff : -w[i]*diff;
		if (c && q>c[i]) q = c[i];
		cost += q;
	}

	return cost;
}

// returns f(lambda) - f(lambda0)
inline double _GetQuadraticUnary(double a, double b, double lambda0, double lambda)
{
	return 0.5*a*(lambda-lambda0)*(lambda+lambda0) - b*(lambda-lambda0);
}

/////// same for quadratic
inline double GetQuadraticUnary(double* unary, int breakpoint_num, double lambda)
{
	if (breakpoint_num == 0) return 0.5*unary[0]*lambda*lambda - unary[1]*lambda;
	if (lambda <= unary[2]) return _GetQuadraticUnary(unary[0], unary[1], unary[2], lambda);
	double y = 0;
	for (unary+=3; breakpoint_num>1; unary+=3, breakpoint_num--)
	{
		if (lambda <= unary[2]) break; 
		y += _GetQuadraticUnary(unary[0], unary[1], unary[-1], unary[2]);
	}
	return y + _GetQuadraticUnary(unary[0], unary[1], unary[-1], lambda);
}

inline double EvaluateTVPiecewiseQuadratic(int n, double* unaries, int* unaries_breakpoint_num, double* w, double* c, double* solution)
{
	int i;
	double cost = 0;
	for (i=0; i<n; i++)
	{
		cost += GetQuadraticUnary(unaries, unaries_breakpoint_num[i], solution[i]);
		unaries += 3*unaries_breakpoint_num[i]+2;
	}
	for (i=0; i<n-1; i++)
	{
		double diff = solution[i+1] - solution[i];
		double q = (diff >= 0) ? w[i]*diff : -w[i]*diff;
		if (c && q>c[i]) q = c[i];
		cost += q;
	}

	return cost;
}






#endif
