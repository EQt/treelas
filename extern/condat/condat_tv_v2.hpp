#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void TV1D_denoise_v2(const double* input,
                     double* output,
                     unsigned int width,
                     const double lambda);

#ifdef __cplusplus
}
#endif
