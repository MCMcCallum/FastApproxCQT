//
// Created: 03/25/17 by Matthew McCallum
//
// Class for computing a fast wavelet transform.
//



#include "FastWavelet.h"
#include "STFTAnalysis.h"
#include "FFT.h"
#include <iostream>



using namespace cupcake;



FastWavelet::FastWavelet( float overlap, const std::vector<float>& window ) :
    mSTFT( new STFTAnalysis<FastWavelet::FAST_WAVELET_FFT_SIZE>( overlap, window ) ),
    mOutputBuffer( FastWavelet::FAST_WAVELET_FFT_SIZE, 0.0 )
///
/// Constructor
///
{
    std::cout << "constructed...";
}

FastWavelet::~FastWavelet() = default;

float FastWavelet::PushSamples( const std::vector<float>& audio )
///
/// Push samples to be analysed. Currently dummy function just for testing
/// python bindings.
///
{
    std::cout << "In here...\n";
    
    // Circular buffer, window, and STFT
    auto& stft_output = mSTFT->PushSamples( audio );
    
    std::cout << "\nOutput is " << stft_output.size() << " frames.\n";
    std::cout << "First frame is " << stft_output[0].size() << " elements.\n";
    for( auto& element : stft_output[0] )
    {
        std::cout << element.real() << " + " << element.imag() << "j,\n";
    }
    
    // Take magnitude
    spectral_magnitude( stft_output[0].data(), mOutputBuffer.data(), FastWavelet::FAST_WAVELET_FFT_SIZE );
    
    return mOutputBuffer[0];
    

    
    // Smearing
    
    // Additional stuff
    
    // Find peaks
    
    // Adjust phase
    
    // Subtract prediction
    
    // IFFT
    
    // Synthesise
}