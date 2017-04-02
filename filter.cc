#include <string>
#include <vector>
#include <fftw3.h>
#include "util.h"
#include "filter.h"

void fft_filter_one(const double *in, double **out, const WindowFunction &window,
                    size_t length, const std::vector<double> &factors) {
    using namespace std;
    size_t window_size = window.size();
    *out = new double[window_size];
    double *window_data = new double[window_size];
    fftw_complex *fd = new fftw_complex[window_size];
    for (size_t i = 0;
         i < ((length < window_size) ? length : window_size);
         ++i) {
        window_data[i] = in[i] * window.window(i);
    }
    fftw_plan plan = fftw_plan_dft_r2c_1d(window_size, window_data, fd, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    for (size_t i = 0; i < window_size / 2 + 1; ++i) {
        fd[i][0] *= factors[i];
        fd[i][1] *= factors[i];
    }
    fftw_plan plan_i = fftw_plan_dft_c2r_1d(window_size, fd, window_data, FFTW_ESTIMATE);
    fftw_execute(plan_i);
    fftw_destroy_plan(plan_i);
    for (size_t i = 0; i < window_size; ++i) {
        (*out)[i] = window_data[i] / window_size;
    }
    delete window_data;
    delete[] fd;
}

void fft_filter(const double *in, double **out, const WindowFunction &window,
                size_t length, const std::vector<double> &factors) {
    using namespace std;
    size_t window_size = window.size();
    size_t chunk_size = window.chunk_size();
    size_t window_number = ceil(length / double(chunk_size));
    size_t new_length = (window_number - 1) * chunk_size + window_size;
    double *data = new double[new_length];
    copy(in, in + length, data);
    *out = new double[new_length];
    for (size_t i = 0; i < window_number; ++i) {
        double *buffer = nullptr;
        fft_filter_one(data + i * chunk_size, &buffer, window, window_size, factors);
        for (size_t j = 0; j < window_size; ++j) {
            (*out)[i*chunk_size+j] += buffer[j];
        }
        delete buffer;
    }
    delete data;
}