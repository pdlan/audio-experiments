audio_experiment:
	clang++ main.cc resample.cc util.cc wavwriter.cc shift_pitch.cc libkissfft.a -std=c++11 -g