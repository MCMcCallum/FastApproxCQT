//
// Created by: Matthew McCallum
// 4th June 2017
//
// Fast implementation of the approximate CQT
//

#ifndef CUPCAKE_CQT_H
#define CUPCAKE_CQT_H

namespace cupcake
{
    
template< FFT_SIZE >
class CQT
{
    
public:
    
    CQT();
    ~CQT();
    
    void Apply( std::vector< std::array< double, FFT_SIZE > > signal );
    
private:
    
    static constexpr size_t GetIOSize() { return get_output_FFT_size( FFTSize ); };
    
    std::vector< double > mFilterCoefficients;
    
    void CalculateFilterCoefficients();
    
};

template< FFT_SIZE >
CQT::CQT() :
    mFilterCoefficients( FFT_SIZE/2, 0.0 )
///
/// Constructor.
///
{
    CalculateFilterCoefficients();
}
    
template< FFT_SIZE >
CQT::Apply( std::vector< std::array< std::complex< double >, FFT_SIZE > > signal )
///
/// Applies a fast CQT operation to a set of STFT coefficients.
/// This is performed by double filtering the complex coefficients across frequency
/// with a single pole exponential moving average.
/// As such this is not a real CQT, but much like the CQT it has an effective window
/// size that grows smaller with increasing frequency.
///
/// @param signal
///  A 2D array of complex valued STFT coefficients.
///  The first index corresponds to time and the second index corresponds to frequency.
///
{
    for( int frame=0; frame<signal.size(); frame++ )
    {
        signal[0] = {0.0, 0.0};
        
        // convolve forwards
        for( int FFT_index=1; FFT_index<( FFT_SIZE/2 ); ++FFT_index )
        {
            auto& sig_frame = signal[frame];
            auto& fc = mFilterCoefficients[FFT_index];
            sig_frame[FFT_index].real() = fc*sig_frame[FFT_index-1].real() + ( 1.0 - fc )*sig_frame[FFT_index].real();
            sig_frame[FFT_index].imag() = fc*sig_frame[FFT_index-1].imag() + ( 1.0 - fc )*sig_frame[FFT_index].imag();
        }
        
        sig_frame[FFT_SIZE/2-1].real() = fc*sig_frame[FFT_SIZE/2-1].real() + ( 1.0 - fc )*sig_frame[FFT_SIZE/2-1].real();
        sig_frame[FFT_SIZE/2-1].imag() = fc*sig_frame[FFT_SIZE/2-1].imag() + ( 1.0 - fc )*sig_frame[FFT_SIZE/2-1].imag();
        
        // convolve backwards
        for( int FFT_index=( FFT_SIZE/2 - 1 ); FFT_index>1; --FFT_index )
        {
            auto& sig_frame = signal[frame];
            auto& fc = mFilterCoefficients[FFT_index];
            sig_frame[FFT_index-1].real() = fc*sig_frame[FFT_index].real() + ( 1.0 - fc )*sig_frame[FFT_index-1].real();
            sig_frame[FFT_index-1].imag() = fc*sig_frame[FFT_index].imag() + ( 1.0 - fc )*sig_frame[FFT_index-1].imag();
        }
        
        sig_frame[sig_frame.size()-1] = {0,0}; // Remove the Nyquist component - this is dependent on the way the output of the FFT operation is formatted.
    }
}
    
template< FFT_SIZE >
void CalculateFilterCoefficients()
///
/// Calculates spectral coefficients by logarithmically mapping FFT indices to a set of
/// empirically derived coefficients. All fractional indices are linearly interpolated.
///
{
    float spectral_breaks[13] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.7f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f };
    
    mFilterCoefficients.resize( 2 );
    
    float interp_point;
    int spectral_break_index;
    for (int i=2; i<FFT_SIZE/2; i++)
    {
        interp_point = ( log( static_cast< double >( i ) )/log( static_cast< float >( 2.0 ) ) - 1.0 );
        spectral_break_index = static_cast< int >( interp_point );
        interp_point -= spectral_break_index;
        mFilterCoefficients.push_back( spectral_breaks[spectral_break_index]*( 1.0 - interp_point ) + spectral_breaks[spectral_break_index+1]*interp_point );
    }
}
    
} // namespace cupcake

#endif // CUPCAKE_CQT_H
