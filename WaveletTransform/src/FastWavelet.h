//
// Created: 03/25/17 by Matthew McCallum
//
// Class for computing a fast wavelet transform.
//

// @todo [matt.mccallum 10.02.17] There could be some improvement here by implementing a sort
//                                of chain-of-command design pattern using templates so that
//                                the process of taking inputs and passing them on to the next
//                                block is all accounted for.
//                                For example, have a templated signal processing block with
//                                arbitrary inputs and outputs. This class doesn't care what
//                                the signal processing blocks are, it just needs to take some
//                                input or previous output and pass it on to the next block.

#ifndef CUPCAKE_FAST_WAVELET_H
#define CUPCAKE_FAST_WAVELET_H

// In module includes.
// None.

// Third party includes.
#include "FFT.h"

// Std lib includes.
#include <vector>
#include <memory>

namespace cupcake
{
    
template< size_t FFT_SIZE > class STFTAnalysis;
template< size_t FFT_SIZE > class FastCQT;

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
    std::vector< std::complex< float > > GetCQTCoeffs();

private:
    
    //
    // Mechanics
    //
    std::unique_ptr<STFTAnalysis<FAST_WAVELET_FFT_SIZE>> mSTFT;
    std::unique_ptr<FastCQT<FAST_WAVELET_FFT_SIZE>> mCQT;
    
    //
    // Data
    //
    std::vector<float> mOutputBuffer;
};

} // namespace cupcake

#endif // CUPCAKE_FAST_WAVELET_H