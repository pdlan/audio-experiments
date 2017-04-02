#ifndef FILTER_H
#define FILTER_H
#include <string>
#include <vector>
#include "util.h"

void fft_filter(const double *in, double **out, const WindowFunction &window,
                size_t length, const std::vector<double> &factors);
#endif