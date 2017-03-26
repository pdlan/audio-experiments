audio_experiment:
	clang++ main.cc resample.cc util.cc wavwriter.cc shift_pitch.cc -lfftw3 -std=c++11 -o audio-tool