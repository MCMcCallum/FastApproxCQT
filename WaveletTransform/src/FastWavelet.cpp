//
// Created: 03/25/17 by Matthew McCallum
//
// Class for computing a fast wavelet transform.
//

#include "FastWavelet.h"

using namespace analysis;

FastWavelet::FastWavelet() :
	mOutputBuffer( 10, std::vector<double>( 10, 3.0 ) )
///
/// Constructor
///
{
	
}

float FastWavelet::PushSamples( const std::vector<double>& audio )
///
/// Push samples to be analysed. Currently dummy function just for testing
/// python bindings.
///
{
	return audio[0];
    
    // Circular buffer
    
    // Window
    
    // STFT
    
    // Smearing
    
    // Additional stuff
    
    // Find peaks
    
    // Adjust phase
    
    // Subtract prediction
    
    // IFFT
    
    // Synthesise
}