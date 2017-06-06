///
/// Created by: Matthew McCallum
/// 15th May 2017
///
/// Class for efficient STFT synthesis via the overlap-add method of a signal - from STFT spectra to signal.
///

#ifndef CUPCAKE_STFT_SYNTHESIS_H
#define CUPCAKE_STFT_SYNTHESIS_H

// In Module includes
#include "STFTAnalysis.h"
#include "OverlapAddBuffer.h"
#include "FFT.h"
#include "vector_functions.h"

// Thirdparty includes
// None.

// Std Lib includes
#include <vector>
#include <complex>
#include <array>

namespace cupcake
{

template< uint64_t FFTSize >
class STFTSynthesis
{

public:
    
    STFTSynthesis( size_t sample_increment, const std::vector< double >& window );
    STFTSynthesis( double overlap, const std::vector< double >& window );
    STFTSynthesis( const STFTAnalysis< FFTSize >& analysis );
    ~STFTSynthesis();
    
    static constexpr size_t GetInputSize() { return get_output_FFT_size( FFTSize ); };
    
    const std::vector< double >& PushFrames( const std::vector< std::array< std::complex< double >, GetInputSize() > >& STFTFrames );
    
    const size_t GetIncrement() const;
    const std::vector< double >& GetWindow() const;
    const size_t GetWinLen() const;
    
private:
    
    //
    // Configuration
    //
    const size_t mIncrement;
    const std::vector< double > mWindow;
    const size_t mWinLen;
    
    //
    // Data
    //
    std::vector< double > mTempBuffer;
    OverlapAddBuffer< double > mOverlapAddBuffer;
    std::vector< double > mOutputBuffer;
    
    //
    // Mechanics
    //
    FFTConfig mFFTConfig;
    double mNormalisationMult;
    
    //
    // Constants
    //
    static const size_t MAX_NUM_INPUT_FRAMES = 1000;
    
    //
    // Helpers
    //
    static double ComputeNormalizationMultiplier( size_t sample_increment, const std::vector< double >& window );
    void CheckParameters();
    
};

template< uint64_t FFTSize >
STFTSynthesis< FFTSize >::STFTSynthesis( size_t sample_increment, const std::vector< double >& window ) :
    mIncrement( sample_increment ),
    mWindow( window ),
    mWinLen( window.size() ),
    mTempBuffer( FFTSize ),
    mOverlapAddBuffer( MAX_NUM_INPUT_FRAMES*(mIncrement - 1) + mWinLen ),
    mOutputBuffer( mOverlapAddBuffer.Size() - mWinLen + mIncrement ),
    mFFTConfig(),
    mNormalisationMult( ComputeNormalizationMultiplier( mIncrement, mWindow ) )
///
/// Constructor.
///
/// @param sample_increment
///  The number of samples between each ovelap add position at the output.
///
/// @param window
///  The window used for the analysis operation that this object is supposed
///  to synthesis the (modified) output of.
///
{
    CheckParameters();
    make_FFT( FFTSize, mFFTConfig );
}
    
template< uint64_t FFTSize >
STFTSynthesis< FFTSize >::STFTSynthesis( double overlap, const std::vector< double >& window ) :
    mIncrement( static_cast< size_t >( ( 1-overlap )*window.size() ) ),
    mWindow( window ),
    mWinLen( window.size() ),
    mTempBuffer( FFTSize ),
    mOverlapAddBuffer( MAX_NUM_INPUT_FRAMES*(mIncrement - 1) + mWinLen ),
    mOutputBuffer( mOverlapAddBuffer.Size() - mWinLen + mIncrement ),
    mFFTConfig(),
    mNormalisationMult( ComputeNormalizationMultiplier( mIncrement, mWindow ) )
///
/// Constructor.
///
/// @param overlap
///  The fraction of frame length by which frames overlap when overlap-added to
///  synthesise the output.
///
/// @param window
///  The window used for the analysis operation that this object is supposed
///  to synthesis the (modified) output of.
///
{
    CheckParameters();
    make_FFT( FFTSize, mFFTConfig );
}

template< uint64_t FFTSize >
STFTSynthesis< FFTSize >::STFTSynthesis( const STFTAnalysis< FFTSize >& analysis ) :
    mIncrement( analysis.GetIncrement() ),
    mWindow( analysis.GetWindow() ),
    mWinLen( analysis.GetWinLen() ),
    mTempBuffer( FFTSize ),
    mOverlapAddBuffer( MAX_NUM_INPUT_FRAMES*(mIncrement - 1) + mWinLen ),
    mOutputBuffer( mOverlapAddBuffer.Size() - mWinLen + mIncrement ),
    mFFTConfig(),
    mNormalisationMult( ComputeNormalizationMultiplier( mIncrement, mWindow ) )
///
/// Constructor.
///
/// @param analysis
///  An STFT analysis object that this synthesis object is supposed to resynthesis
///  the output of (after possible modification). Thus forming a complete STFT analysis
///  synthesis framework.
///
{
    CheckParameters();
    make_FFT( FFTSize, mFFTConfig );
}

template< uint64_t FFTSize >
STFTSynthesis< FFTSize >::~STFTSynthesis()
///
/// Destructor.
///
{
    destroy_FFT( mFFTConfig );
}
    
template< uint64_t FFTSize >
const std::vector< double >& STFTSynthesis< FFTSize >::PushFrames( const std::vector< std::array< std::complex< double >, GetInputSize() > >& STFTFrames )
///
/// Push frames into the STFT synthesis object and synthesise the corresponding signal
/// in the frequency domain using the overlap-add method. Currently this employs no
/// synthesis window and it relies on this object checking the window for the perfect
/// reconstruction property with a normalisation scalar.
///
/// @param STFTFrames
///  Several successive short term spectra of which to take the IFFT and perform the overlap-
///  add operation. Spectra with increasing index are consecutive in time.
///
/// @return
///  A vector of the complete portion of the output signal from the overlap-add operation that has
///  no further overlapping windows to be added to.
///
{
    
    for( auto& spec : STFTFrames )
    {
        
        // IFFT
        mTempBuffer.resize( FFTSize );
        IFFT_not_in_place( spec.data(), mTempBuffer.data(), mFFTConfig );
        
        // Truncate
        mTempBuffer.resize( mWinLen );
        
        // @todo [matthew.mccallum 05.16.17] : No synthesis window used here. There should really be one.
        
        // Overlap-Add
        mOverlapAddBuffer.PushSamples( mTempBuffer );
        mOverlapAddBuffer.IncrementWritePosition( mIncrement );
        
    }
    
    // Write to output
    mOutputBuffer.resize( mOverlapAddBuffer.NumSamples() );
    mOverlapAddBuffer.Read( mOutputBuffer );
    mOverlapAddBuffer.PopFront( mOutputBuffer.size() );
    vec_mult_const_in_place( mOutputBuffer.data(), mNormalisationMult, mOutputBuffer.size() );
    
    return mOutputBuffer;

}

template< uint64_t FFTSize >
const size_t STFTSynthesis< FFTSize >::GetIncrement() const
///
/// Get the number of samples incremented between each frame in the overlap-add operation
///
/// @return
///  The number of samples incremented between each frame in the overlap-add operation
///
{
    return mIncrement;
}

template< uint64_t FFTSize >
const std::vector< double >& STFTSynthesis< FFTSize >::GetWindow() const
///
/// Get the windowing function based on which this STFT synthesis object truncates (accoridng to
/// its length) and scales (according to the function) the output of the IFFT operation.
///
/// @return
///  The windowing function used in the analysis of the signal for which this STFT synthesis
///  object is a part of.
///
{
    return mWindow;
}

template< uint64_t FFTSize >
const size_t STFTSynthesis< FFTSize >::GetWinLen() const
///
/// Get the length of the windowing function used in the corresponding STFT analyis operation
/// for this STFT synthesis object.
///
/// @return
///  The windowing function length.
///
{
    return mWinLen;
}
    
template< uint64_t FFTSize >
double STFTSynthesis< FFTSize >::ComputeNormalizationMultiplier( size_t sample_increment, const std::vector< double >& window )
///
/// Compute the multiplier by which the output of the overlap-add operation must be scaled for
/// perfect reconstruction in the STFT framework this object is a part of.
///
/// @param sample_increment
///  The number of samples progressed between each frame in the overlap-add operation.
///
/// @param window
///  The window used in the STFT analysis operation for the STFT framework this object is a part of.
///
{
    double amplitude = 0.0;
    for( size_t sample_num=0; sample_num<window.size(); sample_num+=sample_increment )
    {
        amplitude += window[sample_num];
    }
    
    return 1.0/amplitude;
}
    
template< uint64_t FFTSize >
void STFTSynthesis< FFTSize >::CheckParameters()
///
/// Check the parameters of this STFT synthesis object allow for perfect reconstruction.
///
{
    // Ensure integer number of windows overlapping
    assert( ( mWindow.size() % mIncrement ) == 0 );
    
    // Ensure number of overlapping windows at any point is a power of two.
    size_t num_overlap = mWindow.size()/mIncrement;
    while( num_overlap >>= 1 )
    {
        assert( ( ( num_overlap & 1 )==0 ) || num_overlap==1 ); // number of overlapping windows must be a power of 2.
    }
}
    
} // namespace cupcake

#endif // CUPCAKE_STFT_SYNTHESIS_H

