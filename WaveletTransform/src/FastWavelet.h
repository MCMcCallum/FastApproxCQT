//
// Created: 03/25/17 by Matthew McCallum
//
// Class for computing a fast wavelet transform.
//

#ifndef CUPCAKE_FAST_WAVELET_H
#define CUPCAKE_FAST_WAVELET_H

#include "FFT.h"

#include <vector>
#include <memory>

namespace cupcake
{
    
template< size_t FFT_SIZE > class STFTAnalysis;

class FastWavelet
///
/// Fast wavelet analyser.
///
{
    static const size_t FAST_WAVELET_FFT_SIZE = 4096;

public:
    FastWavelet( float overlap, const std::vector<float>& window );
	~FastWavelet();
    
    static const size_t mOutputSize = get_output_FFT_size( FAST_WAVELET_FFT_SIZE );
    
    std::vector< std::array< std::complex< float >, mOutputSize > >& PushSamples( const std::vector<float>& audio );
    
    std::vector< float > GetWindow();

private:
    
    // Parameters
    
    // Mechanics
    std::unique_ptr<STFTAnalysis<FAST_WAVELET_FFT_SIZE>> mSTFT;
    
    // Data
    std::vector<float> mOutputBuffer;
};

} // namespace cupcake

#endif // CUPCAKE_FAST_WAVELET_H