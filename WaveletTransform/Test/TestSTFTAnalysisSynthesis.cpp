//
// Created by: Matthew McCallum
// 4th June 2017
//
// Test class for the complete STFT analysis synthesis framework.
//

// In module includes
#include "STFTAnalysis.h"
#include "STFTSynthesis.h"

// Thirdparty includes
#include "gtest/gtest.h"
#include "sig_gen.h"

// Std Lib includes
#include <vector>
#include <functional>
#include <numeric>
#include <algorithm>

using namespace cupcake;

struct sinusoid
{
    float freq;
    float phase;
    float mag;
    std::vector< float > sig;
};

class STFTAnalysisSynthesisTest : public ::testing::Test
{
    
protected:
    
    const size_t MAX_INPUT_SIZE_SAMPLES = 44100*20;
    const size_t NUM_INPUT_SINUSOIDS = 3;
    const float MIN_SINUSOID_FREQ_SPACING = 0.05;
    const size_t WINDOW_LENGTH = 1024;
    
    virtual void SetUp()
    ///
    /// Before all the tests, create some signals to test with.
    ///
    {
        // Create white noise input
        veclib::seed_rand();
        input_uniform_noise.resize( MAX_INPUT_SIZE_SAMPLES );
        std::generate( input_uniform_noise.begin(), input_uniform_noise.end(), std::bind( &veclib::make_random_number, -1.0, 1.0 ) );
        
        // Create sinusoid inputs
        for( int i=0; i<NUM_INPUT_SINUSOIDS; i++ )
        {
            sinusoid the_sinusoid;
            // Find frequencies sufficiently far apart to avoid interaction for tests.
            bool found_freq = false;
            while( !found_freq )
            {
                the_sinusoid.freq = veclib::make_random_number( 0.0, 0.5 );
                found_freq = true;
                for( auto& signal : input_sinusoids )
                {
                    found_freq &= std::abs( signal.freq - the_sinusoid.freq ) > MIN_SINUSOID_FREQ_SPACING;
                    found_freq &= std::abs( signal.freq - the_sinusoid.freq ) < ( 0.5 - MIN_SINUSOID_FREQ_SPACING );
                }
            }
            // Set the rest of the sinusoid parameters
            the_sinusoid.mag = veclib::make_random_number( 0.0, 1.0 );
            the_sinusoid.phase = veclib::make_random_number( 0.0, 2*M_PI );
            the_sinusoid.sig.resize( MAX_INPUT_SIZE_SAMPLES );
            veclib::fill_vector_sine( the_sinusoid.sig, the_sinusoid.freq, the_sinusoid.phase, the_sinusoid.mag );
            input_sinusoids.push_back( the_sinusoid );
        }
        
        // Create the Hamming window
        hamming_window.resize( WINDOW_LENGTH );
        veclib::hamming( hamming_window );
    }
    
    std::vector< float > input_uniform_noise; // Vectors for storing the input for all tests.
    std::vector< sinusoid > input_sinusoids;
    std::vector< float > hamming_window;
    
};

TEST_F( STFTAnalysisSynthesisTest, test_sinusoid_reconstruction )
///
/// Test that when we put a sinusoidal signal through both the STFT analysis and synthesis
/// objects we get the same sinusoid at the output.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTAnalysisSynthesisTest, test_noise_reconstruction )
///
/// Test that when we put a noise signal through both the STFT analysis and synthesis
/// objects we get the same sinusoid at the output.
///
{
    static const size_t FFT_SIZE = 1024;        // -> Number of input samples to the FFT operation (after zero-padding)
    
    const float OVERLAP = 0.875;               // -> The level of the expected DC output
    const float TOLERANCE = 0.00001;           // -> The allowable deviation of the output from the expected signal
    const float INPUT_NUM_SAMPLES = 44100*9;   // -> The number of samples to put through the analysis/synthesis framework
    const size_t PUSH_NUM_FRAMES = 9;           // -> The number of frames to push at a time into the STFT synthesis object
    
    STFTAnalysis< FFT_SIZE > analyzer( OVERLAP, hamming_window );
    STFTSynthesis< FFT_SIZE > synthesizer( analyzer );
    
    // Analyse input signal
    std::vector< float > input_signal( input_uniform_noise.begin(), input_uniform_noise.begin() + INPUT_NUM_SAMPLES );
    const std::vector< std::array< std::complex< float >, analyzer.GetOutputSize() > >& output = analyzer.PushSamples( input_signal );
    std::vector< float > final_output;
    
    // Push analysed signal into the synthesis object.
    size_t num_frames_pushed = 0;
    while( num_frames_pushed < output.size() )
    {
        size_t frames_to_push = std::min( output.size() - num_frames_pushed, PUSH_NUM_FRAMES );
        const std::vector< std::array< std::complex< float >, synthesizer.GetInputSize() > > input_frames( output.begin() + num_frames_pushed, output.begin() + num_frames_pushed + frames_to_push );
        const std::vector< float >& this_output = synthesizer.PushFrames( input_frames );
        final_output.insert( final_output.end(), this_output.begin(), this_output.end() );
        num_frames_pushed += frames_to_push;
    }
    
    // Compare input/output
    for( size_t samp=WINDOW_LENGTH; samp<( final_output.size() - WINDOW_LENGTH ); samp++ )
    {
        EXPECT_NEAR( final_output[samp], input_signal[samp], TOLERANCE );
    }
}

TEST_F( STFTAnalysisSynthesisTest, test_sinusoid_filtering )
///
/// Test that when we input a combination of sinusoids to the STFT analysis object
/// and then notch filter one of the sinusoids in the frequency domain, we get the
/// remaining sinusoids resynthesised closely at the output.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
    
    // Some code below might help with creating the inputs to the test.
    //    // Create summation of sinusoids input
    //    combined_sinusoids_all.resize( 0 );
    //    combined_sinusoids_all.resize( MAX_INPUT_SIZE, 0.0 );
    //    for( auto& current_sinusoid : input_sinusoids )
    //    {
    //        std::transform( current_sinusoid.sig.begin(),
    //                       current_sinusoid.sig.end(),
    //                       combined_sinusoids_all.begin(),
    //                       combined_sinusoids_all.begin(),
    //                       std::plus< float >() );
    //    }
    //
    //    // Create summation of sinusoids input missing the last sinusoid
    //    combined_sinusoids_minus_1.resize( 0 );
    //    combined_sinusoids_minus_1.resize( MAX_INPUT_SIZE, 0.0 );
    //    for( int i=0; i<( NUM_INPUT_SINUSOIDS-1 ); ++i )
    //    {
    //        std::transform( input_sinusoids[i].sig.begin(),
    //                       input_sinusoids[i].sig.end(),
    //                       combined_sinusoids_minus_1.begin(),
    //                       combined_sinusoids_minus_1.begin(),
    //                       std::plus< float >() );
    //    }
}
