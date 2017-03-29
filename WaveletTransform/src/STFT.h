//
// Created: 03/25/17 by Matthew McCallum
//
// Standard STFT analysis class.
//

#ifndef CUPCAKE_STFT_H
#define CUPCAKE_STFT_H

#include "AudioBuffer.h"
#include "VectorFunctions.h"
#include <vector>
#include <array>
#include <complex>

namespace cupcake
{

template< size_t FFT_SIZE >
class STFT
{
    
public:
    
    static const size_t DEFAULT_MAX_INPUT_SIZE = 44100*5; // 5 seconds at 44.1kHz
    
    STFT( size_t win_len,
          size_t hop,
          const std::vector<float>& window,
          size_t max_input_size = DEFAULT_MAX_INPUT_SIZE );
    
    ~STFT();
    
    const std::vector<std::array<std::complex<float>, FFT_SIZE>>& PushSamples( const std::vector<float>& samples );
    
private:
    
    // Parameters
    const size_t mWinLen;
    const size_t mHop;
    const std::vector<float> mWindow;
    
    // Mechanics
    FFT_settings mFFT;
    std::vector<float> mWorkingBuffer;
    
    // Data
    AudioBuffer<float> mInputBuffer;
    std::vector<std::array<std::complex<float>, FFT_SIZE>> mOutputBuffer;
    
};
    
    
    
template< size_t FFT_SIZE >
STFT<FFT_SIZE>::STFT( size_t win_len,
                      size_t hop,
                      const std::vector<float>& window,
                      size_t max_input_size ) :
mWinLen( win_len ),
mHop( hop ),
mWindow( window ),
mFFT( prepare_FFT( FFT_SIZE ) ),
mWorkingBuffer( FFT_SIZE, 0.0 ),
mInputBuffer( max_input_size + mWinLen - 1 ),   // The maximum input plus the number of possible left over samples
mOutputBuffer()
///
/// Constructor.
///
/// @param win_len
///  The length of the STFT analysis window.
///
/// @param hop
///  The shift in samples between the start of each frame's analysis
///  window.
///
/// @param window
///  The analysis windowing function - a real valued vector containing
///  the multipliers to be applied to each sample block in the STFT.
///
{
    assert( mWinLen < FFT_SIZE ); // Can't handle FFTs shorter than the window length at the moment.
    
    mOutputBuffer.reserve( ( mInputBuffer.Size() - mWinLen )/mHop );
}

template< size_t FFT_SIZE >
STFT<FFT_SIZE>::~STFT()
///
/// Destructor.
///
{
    destroy_FFT( mFFT );
}

template< size_t FFT_SIZE >
const std::vector<std::array<std::complex<float>, FFT_SIZE>>& STFT<FFT_SIZE>::PushSamples( const std::vector<float>& samples )
///
/// Add samples to the STFT input buffers and perform windowing/transforms on
/// all complete windows in the buffer. Any incomplete windows are kept for future
/// PushSamples, and any samples not required for future samples are removed.
/// Returned are all STFT analysis frames that were able to be analysed this time
/// around.
///
/// @param samples
///  A vector of audio samples to be added to the STFT.
///
/// @return
///  A reference to a vector of complex valued arrays (one contiguous block of memory)
///  representing all STFT frames that were analysed this time around.
///
{
    
    mInputBuffer.AddSamples( samples );
    
    size_t num_windows = ( std::max( mInputBuffer.NumSamples(), mWinLen ) - mWinLen )/mHop;
    mOutputBuffer.resize( num_windows );
    
    size_t output_index = 0;
    
    while( mInputBuffer.NumSamples() >= mWinLen )
    {
        fvec_mult( mInputBuffer.Data(), mWindow.data(), mWorkingBuffer.data(), mWinLen );

        fvec_clear( mWorkingBuffer.data() + mWinLen, FFT_SIZE - mWinLen );

        mInputBuffer.RemoveSamples( mHop );
        
        fvec_fft( mWorkingBuffer.data(), mOutputBuffer[output_index].data(), mFFT );
        
        output_index++;
    }
    
    return mOutputBuffer;
}
    
} // namespace cupcake

#endif // CUPCAKE_STFT_H