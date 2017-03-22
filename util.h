#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <vector>
#include <functional>

typedef std::vector<double *> MultiChannelData;
size_t load_wav(const std::string &filename, MultiChannelData &data);
size_t join_channels(const MultiChannelData &in, double **out, size_t length);
void clear_data(MultiChannelData &data);
#endif