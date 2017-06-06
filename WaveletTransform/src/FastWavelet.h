//
// Created: 03/25/17 by Matthew McCallum
//
// Class for computing a fast wavelet transform.
//

#ifndef CUPCAKE_FAST_WAVELET_H
#define CUPCAKE_FAST_WAVELET_H

#include <vector>
#include <memory>

namespace cupcake
{
    
template< size_t FFT_SIZE > class STFT;

class FastWavelet
///
/// Fast wavelet analyser.
///
{
    static const size_t FAST_WAVELET_FFT_SIZE = 4096;

public:
    FastWavelet( size_t win_len,
                 size_t hop,
                 const std::vector<float>& window );
	~FastWavelet();

    float PushSamples( const std::vector<float>& audio );

private:
    
    // Parameters
    
    // Mechanics
//    std::unique_ptr<STFT<FAST_WAVELET_FFT_SIZE>> mSTFT;
    
    // Data
    std::vector<float> mOutputBuffer;
};

} // namespace cupcake

#endif // CUPCAKE_FAST_WAVELET_H