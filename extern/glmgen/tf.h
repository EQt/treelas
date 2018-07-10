/* Shortened version of
   https://raw.githubusercontent.com/statsmaths/glmgen/glmgen/include/tf.h
*/

#if !__cplusplus
#  include <stdlib.h>   /*  for malloc, free */
#endif

/* Dynamic programming routines */
void tf_dp (int n, double *y, double lam, double *beta);
void tf_dp_weight (int n, double *y, double *w, double lam, double *beta);
