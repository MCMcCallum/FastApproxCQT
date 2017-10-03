//
// Created by: Matthew McCallum
// 4th June 2017
//
// Fast implementation of the approximate CQT
//

#ifndef CUPCAKE_FAST_CQT_H
#define CUPCAKE_FAST_CQT_H

// In module includes
// None.

// Thirdparty includes
#include "sig_gen.h"

// Std Lib includes
#include <math.h>

namespace cupcake
{
    
template< size_t FFT_SIZE >
class FastCQT
{
    
    static const size_t IO_SIZE = veclib::get_output_FFT_size( FFT_SIZE );
    
public:
    
    FastCQT( size_t window_size );
    ~FastCQT();
    
    void ApplyInPlace( std::vector< std::array< std::complex< float >, IO_SIZE > >& signal );
    
    const std::vector< std::complex< float > >& GetFilterCoefficients() const;
    
private:
    
    //
    // Configuration
    //
    std::vector< std::complex< float > > mFilterCoefficients;
    size_t mWinSize;
    
    //
    // Helpers
    //
    void CalculateFilterCoefficients();
    
};

template< size_t FFT_SIZE >
FastCQT<FFT_SIZE>::FastCQT( size_t window_size ) :
    mFilterCoefficients( IO_SIZE, 0.0 ),
    mWinSize( window_size )
///
/// Constructor.
///
{
    CalculateFilterCoefficients();
}
    
template< size_t FFTSize >
FastCQT< FFTSize >::~FastCQT() = default;
    
template< size_t FFT_SIZE >
void FastCQT< FFT_SIZE >::ApplyInPlace( std::vector< std::array< std::complex< float >, IO_SIZE > >& signal )
///
/// Applies a fast CQT operation to a set of STFT coefficients.
/// This is performed by filtering the complex coefficients across frequency
/// with a single pole exponential moving average IIR filter.
/// As such this is not a real CQT, but much like the CQT it has an effective window
/// size that grows smaller with increasing frequency.
///
/// @param signal
///  A 2D array of complex valued STFT coefficients.
///  The first index corresponds to time and the second index corresponds to frequency.
///
/// @todo [matt.mccallum 10.02.17] I might be able to optimize the convolutions here further
///                                with the IPP or other vector/linalg library.
{
    for( auto& sig_frame : signal )
    {
        // convolve forwards
        std::complex<float>* fc = mFilterCoefficients.data();
        std::complex<float>* last_sig_element = sig_frame.data(); // @note [matt.mccallum 10.01.17] It doesn't matter where we start because the first filtering coefficient is 0 (no history).
        for( auto& sig_element : sig_frame )
        {
            sig_element = (*fc)*(*last_sig_element) + ( 1.0f - (*fc) )*sig_element;
            last_sig_element = &sig_element;
            ++fc;
        }
        
        // convolve backwards
        --fc;
        last_sig_element = sig_frame.data() + sig_frame.size() - 1;
        for( auto sig_element=sig_frame.end()-2; sig_element>sig_frame.begin(); --sig_element )
        {
            (*sig_element) = (*fc)*(*last_sig_element) + ( 1.0f - (*fc) )*(*sig_element);
            last_sig_element = &(*sig_element);
            --fc;
        }
        
        sig_frame[sig_frame.size()-1] = {0,0}; // Remove the Nyquist component - this is dependent on the way the output of the FFT operation is formatted.
    }
}
    
template< size_t FFT_SIZE >
void FastCQT< FFT_SIZE >::CalculateFilterCoefficients()
///
/// Calculates the coefficients of the adaptive IIR filter that is applied to the STFT.
/// This is performed via a logarithmic coefficient amplitude between 0 and 1 that has
/// time-shifted via a multiplication with a complex exponential to center the equivalent
/// time window.
///
{
    // Create a logarithmic function for the amplitude of each smoothing coefficient.
    std::vector<float> domain( IO_SIZE );
    veclib::linspace( 0.01, 100.0, domain );    // <= At the moment the range [0.01, 100.0] here is arbitrary, it depends largely
                                                //    on how quickly you want to adapt to a smaller windowing size across frequency.
    std::for_each( domain.begin(), domain.end(),
        []( float& value )
        {
            value = log(value);
        });
    
    // Normalise values in log function from 0 to 1 and apply time shift.
    float time_shift_arg = M_PI/( static_cast<float>( FFT_SIZE )/static_cast<float>( mWinSize ) );
    std::complex<float> time_shift( cos( time_shift_arg ), sin( time_shift_arg ) );
    float min = *std::min_element( domain.begin(), domain.end() );
    float range = *std::max_element( domain.begin(), domain.end() ) - min;
    std::transform( domain.begin(), domain.end(), mFilterCoefficients.begin(),
        [min, range, time_shift]( float value )->std::complex<float>
        {
            return ( value - min )/range*time_shift;
        });
}
    
template< size_t FFT_SIZE >
const std::vector< std::complex< float > >& FastCQT< FFT_SIZE >::GetFilterCoefficients() const
///
/// Getter function for the CQT single pole IIR filter smoothing coefficients.
///
/// @return
///  A vector of coefficients for the single pole IIR filtering of spectra
///  that are input into this class.
///  Each coefficient at index N in this vector is the IIR coefficient used
///  in the filtering step that takes a spectral bin at index N as input.
///
{
    return mFilterCoefficients;
}
    
} // namespace cupcake

#endif // CUPCAKE_FAST_CQT_H
