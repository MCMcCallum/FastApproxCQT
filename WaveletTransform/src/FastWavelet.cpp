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

std::vector< std::array< std::complex< float >, FastWavelet::mOutputSize > >& FastWavelet::PushSamples( const std::vector<float>& audio )
///
/// Push samples to be analysed. Currently dummy function just for testing
/// python bindings.
///
{
    std::cout << "In here...\n";
    
    // Circular buffer, window, and STFT
    auto& stft_output = mSTFT->PushSamples( audio );
    
    return stft_output;
    

    
    // CQT
    
    // Additional stuff
    
    // Find peaks
    
    // Adjust phase
    
    // Subtract prediction
    
    // IFFT
    
    // Synthesise
}

std::vector< float > FastWavelet::GetWindow()
///
/// Returns the windowing function used for STFT analysis in the time domain.
///
/// @return
///  The window as a vector of float values.
///
{
    return mSTFT->GetWindow();
}