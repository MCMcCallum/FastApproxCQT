//
// Created by: Matthew McCallum
// 4th June 2017
//
// Test class for STFTSynthesis class
//

// In module includes
#include "STFTSynthesis.h"
#include "sig_gen.h"

// Thirdparty includes
#include "gtest/gtest.h"

// Std Lib includes
#include <vector>
#include <algorithm>

using namespace cupcake;

struct sinusoid
{
    double freq;
    double phase;
    double mag;
    std::vector< double > sig;
};

class STFTSynthesisTest : public ::testing::Test
{
    
protected:
    
    const size_t MAX_INPUT_SIZE_SAMPLES = 44100*20;
    const size_t NUM_INPUT_SINUSOIDS = 3;
    const double MIN_SINUSOID_FREQ_SPACING = 0.05;
    const size_t WINDOW_LENGTH = 1024;
    
    virtual void SetUp()
    ///
    /// Before all the tests, create some signals to test with.
    ///
    {
        // Create the Hamming window
        hamming_window.resize( WINDOW_LENGTH );
        hamming( hamming_window );
    }
    
    std::vector< double > hamming_window;
    
};

TEST_F( STFTSynthesisTest, test_dc_construction )
///
/// Test that when input with a DC spectrum that the STFT synthesis object creates
/// a signal of the correct DC level.
///
{
    static const size_t FFT_SIZE = 1024;                            // -> Number of input samples to the FFT operation (after zero-padding)
    
    const size_t NUM_INPUT_FRAMES = 100;                            // -> The number of spectrum frames to input into the STFT synthesis object for this test
    const double DC_LEVEL = make_random_number( -1.0, 1.0 );        // -> The level of the expected DC output
    const double OVERLAP = 0.75;                                    // -> The fractional overlap in the STFT synthesis between successive windows
    const size_t NUM_FIRST_INPUT_FRAMES = NUM_INPUT_FRAMES/2 + 3;   // -> The number of frames to push in one operation first, before pushing the remaining frames
    const double TOLERANCE = 0.00001;                               // -> The allowable deviation of the output from the expected signal
    
    // Create STFT snyhtesis object
    STFTSynthesis< FFT_SIZE > synthesizer( OVERLAP, hamming_window );
    
    // Create input
    std::vector< double > time_domain_input( hamming_window );
    time_domain_input.resize( FFT_SIZE, 0.0 );
    FFTConfig fft_config;
    std::vector< std::complex< double > > input_spec( synthesizer.GetInputSize(), 0.0 );
    make_FFT( FFT_SIZE, fft_config );
    FFT_not_in_place( time_domain_input.data(), input_spec.data(), fft_config);
    vec_mult_const_in_place( reinterpret_cast< double* >( input_spec.data() ), DC_LEVEL, input_spec.size()*2 );
    destroy_FFT( fft_config );
    
    // Create DC spectrum
    std::vector< std::array< std::complex< double >, synthesizer.GetInputSize() > > dc_spectral_input( NUM_INPUT_FRAMES, std::array< std::complex< double >, synthesizer.GetInputSize() >() );
    for( auto& frame : dc_spectral_input )
    {
        std::copy( input_spec.begin(), input_spec.end(), frame.begin() );
    }
    
    // Push in part of DC spectrum
    std::vector< std::array< std::complex< double >, synthesizer.GetInputSize() > > input1( dc_spectral_input.begin(), dc_spectral_input.begin() + NUM_FIRST_INPUT_FRAMES );
    std::vector<double> output1( synthesizer.PushFrames( input1 ) );
    
    // Check first few frames are below DC level and monotonically increasing
    double last_sample = 0.0;
    for( size_t samp_index=0; samp_index<=hamming_window.size(); ++samp_index )
    {
        EXPECT_LE( std::abs( output1[samp_index] ), std::abs( DC_LEVEL ) + TOLERANCE );
        EXPECT_GE( std::abs( output1[samp_index] ), std::abs( last_sample ) - TOLERANCE );
        last_sample = output1[samp_index];
    }
    
    // Check the rest is approximately at DC level
    for( size_t samp_index=hamming_window.size()+1; samp_index<output1.size(); ++samp_index )
    {
        EXPECT_NEAR( output1[samp_index], DC_LEVEL, TOLERANCE );
    }
    
    // Push rest of DC spectrum
    std::vector< std::array< std::complex< double >, synthesizer.GetInputSize() > > input2( dc_spectral_input.begin() + NUM_FIRST_INPUT_FRAMES, dc_spectral_input.end() );
    std::vector<double> output2( synthesizer.PushFrames( input2 ) );
    
    // Check output is approximately at DC level
    for( size_t samp_index=0; samp_index<output2.size(); ++samp_index )
    {
        EXPECT_NEAR( output2[samp_index], DC_LEVEL, TOLERANCE );
    }
}

TEST_F( STFTSynthesisTest, test_sinusoid_construction )
///
/// Test that when input with a spectra containing a delta funciton at a certain frequency
/// and phase, that we get a sinusoid at the output of the corresponding frequency and phase.
///
{
    static const size_t FFT_SIZE = 1024;                                                            // -> Number of input samples to the FFT operation (after zero-padding)
    
    const size_t NUM_INPUT_FRAMES = 100;                                                            // -> The number of spectrum frames to input into the STFT synthesis object for this test
    const size_t INPUT_FRAME_CHUNK_SIZE = 11;                                                       // -> The number of frames to push each time into the STFT synthesis object
    const double OVERLAP = 0.875;                                                                   // -> The level of the expected DC output
    const double TOLERANCE = 0.00002;                                                               // -> The allowable deviation of the output from the expected signal
    const double APPROX_SINE_LEVEL = make_random_number( 0.01, 1.0 );                               // -> The approximate amplitude of the sinusoid to be tested (ignoring the window overlap-add normalisation)
    const double PHASE = 0.89;                                                                      // -> The phase of the sinusoid as a fraction of a complete cycle
    const size_t FREQ_BIN = 300;                                                                    // -> The frequency bin at which the delta function for the sinusoid occurs
    const double FREQUENCY = static_cast< double >( FREQ_BIN )/static_cast< double >( FFT_SIZE );   // -> The frequency of the synthesised sinusoid in terms of cycles per sample
    
    // Create synthesis object
    STFTSynthesis< FFT_SIZE > synthesizer( OVERLAP, hamming_window );
    
    // Create sinusoid spectral input
    std::vector< std::array< std::complex< double >, synthesizer.GetInputSize() > > sinusoid_spectral_input( NUM_INPUT_FRAMES, std::array< std::complex< double >, synthesizer.GetInputSize() >() );
    double current_phase = 2*M_PI*PHASE;
    for( auto& frame : sinusoid_spectral_input )
    {
        frame[FREQ_BIN].real( APPROX_SINE_LEVEL/2*FFT_SIZE*cos( current_phase ) );
        frame[FREQ_BIN].imag( APPROX_SINE_LEVEL/2*FFT_SIZE*sin( current_phase ) );
        current_phase += synthesizer.GetIncrement()*FREQUENCY*2*M_PI;
    }
    
    // Process spectra
    std::vector< double > final_output;
    size_t num_frames_pushed = 0;
    while( num_frames_pushed < sinusoid_spectral_input.size() )
    {
        size_t frames_to_push = std::min( sinusoid_spectral_input.size() - num_frames_pushed, INPUT_FRAME_CHUNK_SIZE );
        const std::vector< std::array< std::complex< double >, synthesizer.GetInputSize() > > input_frames( sinusoid_spectral_input.begin() + num_frames_pushed, sinusoid_spectral_input.begin() + num_frames_pushed + frames_to_push );
        const std::vector< double >& this_output = synthesizer.PushFrames( input_frames );
        final_output.insert( final_output.end(), this_output.begin(), this_output.end() );
        num_frames_pushed += frames_to_push;
    }
    
    // Create expected output
    std::vector< double > expected_output( final_output.size(), 0.0 );
    double output_level = *( std::max_element( final_output.begin(), final_output.end() ) );
    fill_vector_sine( expected_output, FREQUENCY, 2*M_PI*PHASE, output_level );
    
    // Compare output
    for( size_t samp=WINDOW_LENGTH; samp<( final_output.size() - WINDOW_LENGTH ); samp++ )
    {
        EXPECT_NEAR( final_output[samp], expected_output[samp], TOLERANCE );
    }
    
}

TEST_F( STFTSynthesisTest, test_nyquist_construction )
///
/// Test that when we input a delta function at nyquist we get an output of alternating
/// positive/negitive values at the correct absolute value.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}

TEST_F( STFTSynthesisTest, test_STFT_parameters )
///
/// Test that the getter functions for parameters of the STFT synthesis object report correct values.
///
{
    // @todo [matthew.mccallum 05.15.17]: Write this test
    ASSERT_TRUE(false);
}
