//
// Created by: Matthew McCallum
// 4th June 2017
//
// Test class for STFTAnalysis class
//

// In module includes
#include "STFTAnalysis.h"
#include "sig_gen.h"

// Thirdparty includes
#include "gtest/gtest.h"

// Std Lib includes
#include <vector>
#include <functional>
#include <numeric>
#include <algorithm>

using namespace cupcake;

struct sinusoid
{
    double freq;
    double phase;
    double mag;
    std::vector< double > sig;
};

class STFTAnalysisTest : public ::testing::Test
{
    
protected:
    
    const size_t MAX_INPUT_SIZE = 44100*20;
    const size_t NUM_INPUT_SINUSOIDS = 3;
    const double MIN_SINUSOID_FREQ_SPACING = 0.05;
    const size_t WINDOW_LENGTH = 1024;

    
    virtual void SetUp()
    ///
    /// Before all the tests, create some signals to test with.
    ///
    {
        // Create white noise input
        seed_rand();
        input_uniform_noise.resize( MAX_INPUT_SIZE );
        std::generate( input_uniform_noise.begin(), input_uniform_noise.end(), std::bind( &make_random_number, -1.0, 1.0 ) );
        
        // Create sinusoid inputs
        for( int i=0; i<NUM_INPUT_SINUSOIDS; i++ )
        {
            sinusoid the_sinusoid;
            // Find frequencies sufficiently far apart to avoid interaction for tests.
            bool found_freq = false;
            while( !found_freq )
            {
                the_sinusoid.freq = make_random_number( 0.0, 0.5 );
                found_freq = true;
                for( auto& signal : input_sinusoids )
                {
                    found_freq &= std::abs( signal.freq - the_sinusoid.freq ) > MIN_SINUSOID_FREQ_SPACING;
                    found_freq &= std::abs( signal.freq - the_sinusoid.freq ) < ( 0.5 - MIN_SINUSOID_FREQ_SPACING );
                }
            }
            // Set the rest of the sinusoid parameters
            the_sinusoid.mag = make_random_number( 0.0, 1.0 );
            the_sinusoid.phase = make_random_number( 0.0, 2*M_PI );
            the_sinusoid.sig.resize( MAX_INPUT_SIZE );
            fill_vector_sine( the_sinusoid.sig, the_sinusoid.freq, the_sinusoid.phase, the_sinusoid.mag );
            input_sinusoids.push_back( the_sinusoid );
        }
        
        // Create the Hamming window
        hamming_window.resize( WINDOW_LENGTH );
        hamming( hamming_window );
    }
    
    std::vector< double > input_uniform_noise; // Vectors for storing the input for all tests.
    std::vector< sinusoid > input_sinusoids;
    std::vector< double > hamming_window;
    
};

TEST_F( STFTAnalysisTest, test_sinusoid_mag )
///
/// Checks the STFT magnitude spectrum for an arbitrary sinusoid with a Hamming window.
///
{
    // FFT parameters
    const size_t INPUT_CHUNK_SIZE = 3000;   // -> The number of samples to push to the STFT analysis object each time
    const size_t FFT_SIZE = 4096;           // -> The size of the FFT operation (in terms of input samples per frame)
    const double FFT_OVERLAP = 0.5;         // -> The fractional overlap between successive STFT windows
    
    // Configure input
    sinusoid& current_sinusoid = input_sinusoids[0];
    std::vector<double> input( current_sinusoid.sig.begin(), current_sinusoid.sig.begin() + INPUT_CHUNK_SIZE );
    
    // Calculate expected results
    const double MAIN_LOBE_MAGNITUDE = 20*log10( std::accumulate( hamming_window.begin(), hamming_window.end(), 0.0 )*current_sinusoid.mag/2.0 ); // sum hamming * num samples / 2
    const double MAIN_LOBE_MAGNITUDE_TOLERANCE = 1; // in dB.
    const double MAIN_LOBE_WIDTH = 4.0/WINDOW_LENGTH; // In cycles per sample - specific to the hamming window.
    const double MAXIMUM_RELATIVE_SIDE_LOBE_MAGNITUDE = -41; // In dB - specific to the hamming window.
    const size_t MAIN_LOBE_BIN_INDEX = std::round( current_sinusoid.freq*FFT_SIZE );
    
    STFTAnalysis< FFT_SIZE > STFT( FFT_OVERLAP, hamming_window );
    
    const std::vector< std::array< std::complex< double >, STFT.GetOutputSize() > >& output = STFT.PushSamples( input );
    std::vector< std::array< double, STFT.GetOutputSize() > > mag_spectra( output.size(), std::array< double, STFT.GetOutputSize() >() );
    
    // Convert to decibel magnitudes.
    for( size_t spec_index=0; spec_index<output.size(); ++spec_index )
    {
        std::transform( output[spec_index].begin(), output[spec_index].end(), mag_spectra[spec_index].begin(),
            []( const std::complex< double >& element )
            {
                return 20*log10( std::abs( element ) );
            }
        );
    }
    
    // Check the main lobe
    for( auto& spec : mag_spectra )
    {
        EXPECT_NEAR( spec[MAIN_LOBE_BIN_INDEX], MAIN_LOBE_MAGNITUDE, MAIN_LOBE_MAGNITUDE_TOLERANCE );
    }
    
    // Check the side lobes
    const double side_lobe_threshold = MAIN_LOBE_MAGNITUDE + MAIN_LOBE_MAGNITUDE_TOLERANCE + MAXIMUM_RELATIVE_SIDE_LOBE_MAGNITUDE;
    for( auto& spec : mag_spectra )
    {
        for( size_t spec_index=0; spec_index<spec.size(); ++spec_index )
        {
            if( ( spec_index < ( MAIN_LOBE_BIN_INDEX - MAIN_LOBE_WIDTH*FFT_SIZE/2 - 1 ) ) ||
                ( spec_index > ( MAIN_LOBE_BIN_INDEX + MAIN_LOBE_WIDTH*FFT_SIZE/2 + 1 ) ) )
            {
                EXPECT_LE( spec[spec_index], side_lobe_threshold );
            }
        }
    }
}

TEST_F( STFTAnalysisTest, test_sinusoid_phase )
///
/// Checks that the phase of a sinusoid is correct at the bin nearest to the main lobe
/// of that sinusoid in the spectrum.
/// Also checks that this progresses correctly over a few frames.
///
{
    // FFT parameters
    const size_t INPUT_CHUNK_SIZE = 3000;   // -> The number of samples to push to the STFT analysis object each time
    const size_t FFT_SIZE = 4096;           // -> The size of the FFT operation (in terms of input samples per frame)
    const double FFT_OVERLAP = 0.75;        // -> The fractional overlap between successive STFT windows
    
    // Configure input
    sinusoid& current_sinusoid = input_sinusoids[0];
    std::vector<double> input( current_sinusoid.sig.begin(), current_sinusoid.sig.begin() + INPUT_CHUNK_SIZE );
    
    // Calculate expected results
    const size_t MAIN_LOBE_BIN_INDEX = std::round( current_sinusoid.freq*FFT_SIZE );
    const size_t SAMPLES_PASSED_BETWEEN_FRAMES = static_cast< size_t >( ( 1 - FFT_OVERLAP )*WINDOW_LENGTH );
    const double FREQ_BIN_OFFSET = current_sinusoid.freq*FFT_SIZE - MAIN_LOBE_BIN_INDEX;
    const double PHASE_AT_FRAME_0 = current_sinusoid.phase + 2*M_PI*FREQ_BIN_OFFSET/FFT_SIZE*( WINDOW_LENGTH - 1.0 )/2.0;
    const double PHASE_INC_BETWEEN_FRAMES = current_sinusoid.freq*2*M_PI*SAMPLES_PASSED_BETWEEN_FRAMES;
    const double PHASE_TOLERANCE = 0.01;
    
    STFTAnalysis<FFT_SIZE> STFT( FFT_OVERLAP, hamming_window );
    
    const std::vector< std::array< std::complex< double >, STFT.GetOutputSize() > >& output = STFT.PushSamples( input );
    std::vector< std::array< double, STFT.GetOutputSize() > > phase_spectra( output.size(), std::array< double, STFT.GetOutputSize() >() );
    
    // Convert to angles.
    for( size_t spec_index=0; spec_index<output.size(); ++spec_index )
    {
        std::transform( output[spec_index].begin(), output[spec_index].end(), phase_spectra[spec_index].begin(),
            []( const std::complex< double >& element )
            {
                return std::arg( element );
            }
        );
    }
    
    // Check the phase at the peak
    double current_phase = PHASE_AT_FRAME_0;
    for( auto& spec : phase_spectra )
    {
        double comparison_phase = current_phase;
        if( comparison_phase > M_PI )
        {
            comparison_phase -= 2*M_PI;
        }
        EXPECT_NEAR( spec[MAIN_LOBE_BIN_INDEX], comparison_phase, PHASE_TOLERANCE );
        current_phase += PHASE_INC_BETWEEN_FRAMES;
        current_phase = fmod( current_phase, 2*M_PI );
    }
}

TEST_F( STFTAnalysisTest, test_multiple_sinusoids )
///
/// Check when multiple sinusoids are input to the STFT we see the corresponding peaks at approximately
/// the correct frequencies and amplitudes.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTAnalysisTest, test_impulse )
///
/// Check when an impulse is input into the STFT we see the correct flat spectra at frames where the
/// impulse occurs.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTAnalysisTest, test_wgn )
///
/// Check that we have approximately the right energy in the spectra when white gaussian noise is
/// input.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTAnalysisTest, test_dc_step )
///
/// Check the indices corresponding to DC is correct when we input DC into the STFT.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTAnalysisTest, test_nyquist )
///
/// Check that when a signal at Nyquist is input into the STFT we get the correct value
/// at the final index.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTAnalysisTest, test_python_reference_wgn )
///
/// Check that when we input the same wgn to numpy's fft and the STFT here, we get the
/// same output.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
    
    ///
    /// Temporary code for printing out STFT as a numpy array.
    /// This is useful for debugging when writing tests.
    ///
    //    std::cout << "\n\n\n x = np.array([";
    //
    //    for( auto& column : output )
    //    {
    //        std::cout << "[";
    //        for( auto& element : column )
    //        {
    //            std::cout << element.real() << "+" << element.imag() << "j,";
    //        }
    //        std::cout << "],";
    //    }
    //    std::cout << "])" << "\n\n\n";
}


TEST_F( STFTAnalysisTest, test_STFT_parameters )
///
/// Test that the getter functions for parameters of the STFT analysis object report correct values.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTAnalysisTest, test_small_inputs )
///
/// Check that when inputting incremental amounts of samples that STFT frames are returned only when
/// push enough for a new complete frame
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}
