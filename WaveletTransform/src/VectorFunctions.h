//
// Created: 03/25/17 by Matthew McCallum
//
// Optimised vectorised functions using the Apple accelerate framework.
// This could easily be swapped out for other platform's vector optimised routines.
//



#ifndef CUPCAKE_VECTOR_FUNCTIONS_H
#define CUPCAKE_VECTOR_FUNCTIONS_H



#include <Accelerate/Accelerate.h>
#include <vector>



namespace cupcake
{

inline void fvec_mult( const float* vec1, const float* vec2, float* result, size_t length )
///
/// Element wise 1D float vector multiplication.
///
/// @param vec1
///  The first factor for multiplication.
///
/// @param vec2
///  The second factor for multiplication.
///
/// @param result
///  A block of memory in which to place the result.
///
/// @param length
///  The number of elements in vec1, vec2 and result.
///
{
    vDSP_vmul( vec1,
               1,
               vec2,
               1,
               result,
               1,
               length );
}
    
inline void fvec_clear( float* vec1, size_t num_elements )
///
/// Sets a number of float values starting at vec1 in memory to 0.0.
///
/// @param vec1
///  A pointer to the start of the vector to be zeroed.
///
/// @param num_elements
///  The number of float values ot zero starting at vec1.
///
{
    vDSP_vclr( vec1,
               1,
               num_elements );
}
    
inline void fvec_complex_magnitude( const std::complex<float>* vec1, float* vec2, size_t length  )
///
/// Get the magnitude of a complex float vector.
/// Because this takes in std::complex values which are interleaved real/imag
/// float values in memory, the values must first be rearranged to suit the
/// optimised absolute value function.
/// It may be quicker to keep complex values as split complex vectors while operating
/// on them (individual real/imag vectors - seperate blocks of memory for real/imag parts).
///
/// @param vec1
///  A pointer to the data in a complex valued vector. It is assumed that this memory is
///  arranged as interleaved real/imag float values.
///
/// @param vec2
///  A pointer to the start of the output vector. It will have half the memory of vec1 and
///  will be a vector of magnitudes for each real/imag pair in vec1.
///
/// @param length
///  The number of real/imag pairs in vec1 to take the magnitude of.
///
{
    DSPSplitComplex intermediate_output { vec2, vec2 + ( length>>1 ) };
    
    vDSP_ctoz( reinterpret_cast<const DSPComplex*>( vec1 ),
               1,
               &intermediate_output,
               1,
               length );
    
    vDSP_zvabs( &intermediate_output,
                1,
                vec2,
                1,
                length ); // This is done in place, not sure if Accelerate is all g with this in all cases.
}
    
    
    
//
// FFT Functions
//
    
struct FFT_settings
///
/// A simple struct for storing an FFT setup and its parameters.
///
{
    size_t Length;
    size_t Log2Length;
    FFTSetup FFT;
};
    
inline FFT_settings prepare_FFT( size_t length )
///
/// Construct an FFT object and memory allocations to be used for future FFT operations.
///
/// @param length
///  The length of the memory to be used in the FFT (in terms of number of floats
///  consumed per analysis).
///
{
    FFT_settings FFT;
    FFT.Length = length;
    
    size_t log_2_length = 0;
    while( length >>= 1 )
    {
        log_2_length++;
        assert( ( ( length & 1 )==0 ) || length==1 ); // Ensure mFFTLen is a power of 2.
    }
    
    FFT.Log2Length = log_2_length;
    FFT.FFT = vDSP_create_fftsetup( log_2_length, kFFTRadix2 );
    
    return FFT;
}
    
inline void fvec_fft( const float* vec1, std::complex<float>* vec2, FFT_settings settings )
///
/// Perform an FFT on a vector of float values.
/// Note this function also goes through the effort of interleaving the FFT output
/// so that it can be returned as a std::complex type. In the future it may be more
/// efficient to not bother with this step and return a struct containing 2 individual
/// real/imag vectors.
///
/// @param vec1
///  The vector of float values to be transformed.
///
/// @param vec2
///  The output of the FFT operations as interleaved real/imag float values.
///  This is packed so that the first float value is the FFT at frequency 0,
///  the second float value is the FFT at frequency fs/2. The remainder are
///  interleaved real/imag float pairs between frequencies 0 and fs/2.
///
/// @param settings
///  A struct containing the length of the FFT, the log base 2 length of the FFT
///  and another struct referencing the preallocated memory for the FFT.
///
{
    
    DSPSplitComplex intermediate_output { reinterpret_cast<float*>( vec2 ), reinterpret_cast<float*>( vec2 ) + ( settings.Length>>1 ) };
    
    // Copy to the output in the right format.
    vDSP_ctoz( reinterpret_cast<const DSPComplex*>( vec1 ),
               1,
               &intermediate_output,
               1,
               settings.Length );
    
    vDSP_fft_zrip( settings.FFT,
                   &intermediate_output,
                   1,
                   settings.Log2Length,
                   FFT_FORWARD );
    
    vDSP_ztoc( &intermediate_output,
               1,
               reinterpret_cast<DSPComplex*>( vec2 ),
               1,
               settings.Length); // May not be able to do this in place as I'm doing here.
}
    
inline void destroy_FFT( FFT_settings settings )
///
/// Destroy and get rid of any pre allocations for an FFT_settings object.
///
/// @param settings
///  An object referencing the memory to be destroyed.
///
{
    vDSP_destroy_fftsetup( settings.FFT );
}

} // namespace cupcake

#endif // CUPCAKE_VECTOR_FUNCTIONS_H