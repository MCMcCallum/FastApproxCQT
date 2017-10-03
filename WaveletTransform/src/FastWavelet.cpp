//
// Created: 03/25/17 by Matthew McCallum
//
// Class for computing a fast wavelet transform.
//

// In module includes
#include "FastWavelet.h"
#include "STFTAnalysis.h"
#include "FastCQT.h"

// Thirdparty includes
// None.

// Std Lib includes
// None.

using namespace cupcake;

FastWavelet::FastWavelet( float overlap, const std::vector<float>& window ) :
    mSTFT( new STFTAnalysis<FastWavelet::FAST_WAVELET_FFT_SIZE>( overlap, window ) ),
    mCQT( new FastCQT<FastWavelet::FAST_WAVELET_FFT_SIZE>( window.size() ) ),
    mOutputBuffer( FastWavelet::FAST_WAVELET_FFT_SIZE, 0.0 )
///
/// Constructor.
///
/// @param overlap
///  The overlap of successive STFT windows as a fraction of windowing length.
///  Overlaps resulting in fractional sample overlaps are floored to a whole number
///  of samples.
///
/// @param window
///  The windowing function of the STFT operation. This vector also implies the windowing
///  length.
///
{
}

FastWavelet::~FastWavelet() = default;

std::vector< std::array< std::complex< float >, FastWavelet::mOutputSize > >& FastWavelet::PushSamples( const std::vector<float>& audio )
///
/// Push samples to be analysed. This performs the STFT on the signal and successively
/// applies a CQT transform on the resulting STFT. Signal samples are buffered for further
/// input.
/// Upon pushing samples, all STFT output frames that can be produced based on these samples and
/// any previously buffered input are returned.
///
/// @param audio
///  A vector of audio samples to be processed.
///
/// @return
///  A contiguous 2D complex valued vector of samples at the output of the fast CQT. 
///
{
    // Circular buffer, window, and STFT.
    auto& stft_output = mSTFT->PushSamples( audio );
 
    // Transform the STFT to have narrower windowing length at higher frequencies.
    mCQT->ApplyInPlace( stft_output );
    
    // Return output.
    return stft_output;
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

std::vector< std::complex< float > > FastWavelet::GetCQTCoeffs()
///
/// Returns the coefficients used for filtering each sample of the STFT.
///
/// @return
///  The CQT coefficients.
///
{
    return mCQT->GetFilterCoefficients();
}