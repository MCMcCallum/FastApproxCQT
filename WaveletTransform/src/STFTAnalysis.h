///
/// Created by: Matthew McCallum
/// 1st May 2017
///
/// Class for efficient STFT analysis operations in C++.
///

#ifndef CUPCAKE_STFT_ANALYSIS_H
#define CUPCAKE_STFT_ANALYSIS_H

// In module includes
#include "AudioBuffer.h"
#include "FFT.h"
#include "vector_functions.h"

// Thirdparty includes
// None.

// Std Lib includes
#include <vector>
#include <complex>
#include <array>
#include <assert.h>

namespace cupcake
{

template< size_t FFTSize >
class STFTAnalysis
{
    
public:

	STFTAnalysis( float overlap, const std::vector< float >& window );
	~STFTAnalysis();
    
    static constexpr size_t GetOutputSize() { return get_output_FFT_size( FFTSize ); };

	std::vector< std::array< std::complex< float >, GetOutputSize() > >& PushSamples( const std::vector< float >& samples );
    
    const size_t GetIncrement() const;
    const std::vector< float >& GetWindow() const;
    const size_t GetWinLen() const;
    

private:

	//
	// Configuration
	//
    const float mOverlap;
	const size_t mIncrement;
	const std::vector< float > mWindow;
	const size_t mWinLen;

	//
	// Data
	//
	AudioBuffer< float > mInputBuffer;
	std::vector< std::array< std::complex< float >, GetOutputSize() > > mOutputBuffer;
    std::vector< float > mWorkingBuffer;
    
    //
    // Mechanics
    //
    FFTConfig mFFTConfig;

	//
	// Constants
	//
	static const size_t INPUT_BUFFER_SIZE = 44100*30;

};

template< size_t FFTSize >
STFTAnalysis< FFTSize >::STFTAnalysis( float overlap, const std::vector< float >& window ) :
	mOverlap( overlap ),
	mIncrement( static_cast< size_t >( ( 1-overlap )*window.size() ) ),
	mWindow( window ),
	mWinLen( window.size() ),
	mInputBuffer( INPUT_BUFFER_SIZE ),
	mOutputBuffer( ( INPUT_BUFFER_SIZE-mWinLen )/mIncrement + 1, std::array< std::complex< float >, GetOutputSize() >() ),
    mWorkingBuffer( FFTSize, 0.0 ),
    mFFTConfig()
///
/// Constructor.
///
/// @param overlap
///  A float value specifying the fraction of window length that is overlapped
///  between windows. The if this results in a fraction of a sample, the overlapping
///  number of samples is truncated to an integer.
///
/// @param window
///  A vector of float values describing the windowing function (and hence windowing
///  length) for the STFT operation.
///
{
    make_FFT( FFTSize, mFFTConfig );
}

template< size_t FFTSize >
STFTAnalysis< FFTSize >::~STFTAnalysis()
///
/// Destructor.
///
{
    destroy_FFT( mFFTConfig );
}

template< size_t FFTSize >
std::vector< std::array< std::complex< float >, STFTAnalysis< FFTSize >::GetOutputSize() > >& STFTAnalysis< FFTSize >::PushSamples( const std::vector< float >& samples )
///
/// Adds samples to previous left over samples at input buffer
/// and performs all the FFT operations it has enough samples
/// for. Returned is a buffer of all FFTs that have been performed
/// for this latest round of samples.
///
/// @param samples
///  Single-channel samples to be added to the input buffer and transformed
///  if there are enough for one or more STFT windows.
///
/// @return
///  Reference to a vector containing all output STFT frames
///
{

	assert( samples.size() < mInputBuffer.SpaceRemaining() ); // Too many samples to fit into input buffer.
	
	// Add samples to input
	mInputBuffer.PushSamples( samples );

	// Prepare output buffer
	size_t numFramesAvailable = mInputBuffer.NumSamples() > mWinLen ? ( mInputBuffer.NumSamples() - mWinLen )/mIncrement + 1 : 0;
	mOutputBuffer.resize( numFramesAvailable ); // @todo [mcmccallum 05/01/17] This will zero initialise all elements, we should try avoid this.

	// Perform the FFTs
	size_t output_frame_idx = 0;
	size_t input_pointer = 0;
	while( ( input_pointer + mWinLen ) < mInputBuffer.NumSamples() )
	{
        
        // Multiply by window
        vec_mult( samples.data() + input_pointer, mWindow.data(), mWorkingBuffer.data(), mWinLen );

		// Perform FFT
        std::array< std::complex< float >, GetOutputSize() >& output_vector = mOutputBuffer[output_frame_idx];
        FFT_not_in_place( mWorkingBuffer.data(), output_vector.data(), mFFTConfig );
        
		// Increment indices for next frame
		output_frame_idx++;
		input_pointer += mIncrement;

	}

	// Clear obsolete samples from the input
	if( numFramesAvailable )
	{
		mInputBuffer.PopFront( ( numFramesAvailable - 1 )*mIncrement + mWinLen );
	}

	return mOutputBuffer;

}
    
template< size_t FFTSize >
const size_t STFTAnalysis< FFTSize >::GetIncrement() const
///
/// Get the number of samples incremented between each analysis window.
///
/// @return
///  The number of samples incremented between each analysis window.
///
{
    return mIncrement;
}
    
template< size_t FFTSize >
const std::vector< float >& STFTAnalysis< FFTSize >::GetWindow() const
///
/// Get the windowing function applied to the input signal before each FFT operation
/// in the STFT analysis procedure
///
/// @return
///  The STFT analysis windowing function.
///
{
    return mWindow;
}

template< size_t FFTSize >
const size_t STFTAnalysis< FFTSize >::GetWinLen() const
///
/// Get the length of the STFT analysis window.
///
/// @return
///  The length of the STFT analysis window.
///
{
    return mWinLen;
}

} // namespace cupcake

#endif // CUPCAKE_STFT_ANALYSIS_H
