//
// Created: 03/25/17 by Matthew McCallum
//
// Class for computing a fast wavelet transform.
//



#include "FastWavelet.h"
#include "STFT.h"



using namespace cupcake;



FastWavelet::FastWavelet( size_t win_len,
                          size_t hop,
                          const std::vector<float>& window ) :
mSTFT( new STFT<FastWavelet::FAST_WAVELET_FFT_SIZE>( win_len, hop, window ) ),
mOutputBuffer( FastWavelet::FAST_WAVELET_FFT_SIZE, 0.0 )
///
/// Constructor
///
{
	
}

FastWavelet::~FastWavelet() = default;

float FastWavelet::PushSamples( const std::vector<float>& audio )
///
/// Push samples to be analysed. Currently dummy function just for testing
/// python bindings.
///
{
    
    // Circular buffer, window, and STFT
    const std::vector<std::array<std::complex<float>, FastWavelet::FAST_WAVELET_FFT_SIZE>>& stft_output = mSTFT->PushSamples( audio );
    
    // Take magnitude
    fvec_complex_magnitude( stft_output[0].data(), mOutputBuffer.data(), FastWavelet::FAST_WAVELET_FFT_SIZE );
    
    return mOutputBuffer[0];
    

    
    // Smearing
    
    // Additional stuff
    
    // Find peaks
    
    // Adjust phase
    
    // Subtract prediction
    
    // IFFT
    
    // Synthesise
}