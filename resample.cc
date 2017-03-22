#include <cmath>
#include <iostream>
#include "resample.h"

void resample_linear(const double* in, double* out, double ratio,
                     size_t in_length, size_t out_length) {
    for (size_t i = 0; i < out_length; ++i) {
        double position = i / ratio;
        size_t c = ceil(position);
        size_t f = floor(position);
        if (c >= in_length) {
            return;
        }
        if (c == f) {
            out[i] = in[c];
        } else {
            double left = in[f];
            double right = in[c];
            out[i] = left + (right - left) * (position - f);
        }
    }
}

void resample_cubic(const double* in, double* out, double ratio,
                    size_t in_length, size_t out_length) {
    size_t part_number = ceil((in_length - 1) / 3.0);
    struct Argument {
        double a,b,c,d;
    };
    Argument *arguments = new Argument[part_number];
    for (size_t i = 0; i < part_number; ++i) {
        double v1 = in[i*3];
        double v2 = (i*3+1) < in_length ? in[i*3+1] : 0;
        double v3 = (i*3+2) < in_length ? in[i*3+2] : 0;
        double v4 = (i*3+3) < in_length ? in[i*3+3] : 0;
        double a = -v1 / 6 + v2 / 2 - v3 / 2 + v4 / 6;
        double b = v1 - 2.5 * v2 + 2 * v3 - v4 / 2;
        double c = -11 * v1 / 6 + 3 * v2 - 1.5 * v3 + v4 / 3;
        double d = v1;
        arguments[i].a = a;
        arguments[i].b = b;
        arguments[i].c = c;
        arguments[i].d = d;
    }
    for (size_t i = 0; i < out_length; ++i) {
        double position = i / ratio;
        size_t c = ceil(position);
        size_t f = floor(position);
        if (c >= in_length) {
            return;
        }
        if (c == f) {
            out[i] = in[c];
        } else {
            size_t part_pos = position / 3;
            double a = arguments[part_pos].a;
            double b = arguments[part_pos].b;
            double c = arguments[part_pos].c;
            double d = arguments[part_pos].d;
            double x = position - (part_pos * 3);
            double v = a*x*x*x+b*x*x+c*x+d;
            out[i] = v;
        }
    }
    delete arguments;
}