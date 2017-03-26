#ifndef SHIFT_PITCH_H
#define SHIFT_PITCH_H
#include "util.h"

size_t shift_pitch(const double *in, double **out, double ratio,
                   const RectWindow &window, size_t length);
size_t shift_pitch(const double *in, double **out, double ratio,
                   const WindowFunction &window, size_t length);
#endif