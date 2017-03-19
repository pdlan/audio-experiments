#ifndef WAVWRITER_H
#define WAVWRITER_H
#include <fstream>
void write_wav(const std::string &filename,
               unsigned int channels, unsigned int samples_per_second,
               double *data, size_t length);
#endif