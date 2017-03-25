//
//
//

#ifndef ANALYSIS_FAST_WAVELET_H
#define ANALYSIS_FAST_WAVELET_H

#include <vector>

namespace analysis{

class FastWavelet
///
/// Fast wavelet analyser.
///
{

public:
	FastWavelet();
	~FastWavelet() {};

	float PushSamples( const std::vector<double>& audio );

private:
	std::vector<std::vector<double>> mOutputBuffer;
    
};

} // namespace analysis

#endif // ANALYSIS_FAST_WAVELET_H