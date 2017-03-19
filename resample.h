#ifndef RESAMPLE_H
#define RESAMPLE_H
void resample_linear(const double* in, double* out, double ratio,
                     size_t in_length, size_t out_length);
void resample_cubic(const double* in, double* out, double ratio,
                    size_t in_length, size_t out_length);
#endif