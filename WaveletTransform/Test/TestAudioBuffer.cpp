//
// Created by: Matthew McCallum
// 4th June 2017
//
// Test class for AudioBuffer class
//

// In module includes
#include "AudioBuffer.h"
#include "sig_gen.h"

// Thirdparty includes
#include "gtest/gtest.h"

// Std Lib includes
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <cstdlib>

using namespace cupcake;

class AudioBufferTest : public ::testing::Test
///
/// Test fixture for audio buffer tests.
/// Creates and holds default input signals.
///
{
protected:
    
    const size_t MAX_INPUT_SIZE = 44100*20;
    
    virtual void SetUp()
    ///
    /// Before all the tests, create an arbitrary input to fill the buffer with.
    ///
    {
        seed_rand();
        input.resize( MAX_INPUT_SIZE );
        std::generate( input.begin(), input.end(), std::bind( &make_random_number, -1.0, 1.0 ) );
    }
    
    std::vector< double > input; // Vector for storing the input for all tests.
    
};

TEST_F( AudioBufferTest, FillDoubleBuffer )
///
/// Tests that when double type samples are pushed into the audio buffer
/// in chunks, the data read from the buffer matches the data that has
/// been pushed in.
///
{
    const size_t BUFFER_SIZE = 44100*3; // -> Size of the data cache in the audio buffer object.
    const size_t PUSH_CHUNK_SIZE = 300; // -> Number of samples per input call to the buffer object.
    
    // Create test object
    AudioBuffer<double> buffer = AudioBuffer<double>( BUFFER_SIZE );
    
    // Push the samples in
    size_t input_pointer = 0;
    while( PUSH_CHUNK_SIZE <= buffer.SpaceRemaining() )
    {
        std::vector<double> temp_input( this->input.begin() + input_pointer, this->input.begin() + input_pointer + PUSH_CHUNK_SIZE );
        buffer.PushSamples( temp_input );
        input_pointer += PUSH_CHUNK_SIZE;
    }
    
    // Check the samples in the buffer
    const double* the_data = buffer.Data();
    size_t num_data = buffer.NumSamples();
    for( int i=0; i<num_data; ++i )
    {
        ASSERT_EQ( the_data[i], this->input[i] );
    }
    
}

TEST_F( AudioBufferTest, ClearSamplesFromBuffer )
///
/// Checks that the number of samples in the buffer add up after clearing
/// various amounts of samples from the buffer.
///
{
    const size_t BUFFER_SIZE = 44100;               // -> Size of the data cache in the audio buffer object.
    const size_t PUSH_CHUNK_SIZE = BUFFER_SIZE/10-3;// -> Number of samples per input call to the buffer object.
    const size_t CLEAR_SIZE = PUSH_CHUNK_SIZE-10;   // -> Number of samples to clear each loop iteration, after adding samples.
    const int NUM_PUSH_CLEAR_CYCLES = 15;           // -> Number of times to fill then clear the buffer in this test.
    
    // Create test object
    AudioBuffer<double> buffer = AudioBuffer<double>( BUFFER_SIZE );
    
    // Clear part of buffer several times
    size_t input_pointer = 0;
    for( int i=1; i<=NUM_PUSH_CLEAR_CYCLES; ++i )
    {
        std::vector<double> temp_input( this->input.begin() + input_pointer, this->input.begin() + input_pointer + PUSH_CHUNK_SIZE );
        buffer.PushSamples( temp_input );
        buffer.PopFront( CLEAR_SIZE );
        
        ASSERT_EQ( buffer.NumSamples(), i*PUSH_CHUNK_SIZE - i*CLEAR_SIZE );
        
        input_pointer += PUSH_CHUNK_SIZE;
    }
    
    // Clear entire buffer
    buffer.PopFront( BUFFER_SIZE );
    ASSERT_EQ( buffer.NumSamples(), 0 );
}

TEST_F( AudioBufferTest, BufferSpaceRemaining )
///
/// Checks that the buffer space remaining is correct after adding samples
/// to the buffer, and clearing samples.
///
{
    const size_t BUFFER_SIZE = 44100*3; // -> Size of the data cache in the audio buffer object.
    const size_t PUSH_CHUNK_SIZE = 300; // -> Number of samples per input call to the buffer object.
    
    // Create test object
    AudioBuffer<double> buffer = AudioBuffer<double>( BUFFER_SIZE );
    
    // Add samples and check the space remaining
    size_t input_pointer = 0;
    while( PUSH_CHUNK_SIZE <= buffer.SpaceRemaining() )
    {
        std::vector<double> temp_input( this->input.begin() + input_pointer, this->input.begin() + input_pointer + PUSH_CHUNK_SIZE );
        buffer.PushSamples( temp_input );
        input_pointer += PUSH_CHUNK_SIZE;
        
        ASSERT_EQ( buffer.SpaceRemaining(), BUFFER_SIZE-input_pointer );
    }
    
    // Clear half the buffer and check the space remaining
    size_t original_space = buffer.SpaceRemaining();
    buffer.PopFront( BUFFER_SIZE/2 );
    ASSERT_EQ( buffer.SpaceRemaining(), original_space + BUFFER_SIZE/2 );
    
    // Add more samples and check the space remaining
    input_pointer = 0;
    while( PUSH_CHUNK_SIZE <= buffer.SpaceRemaining() )
    {
        std::vector<double> temp_input( this->input.begin() + input_pointer, this->input.begin() + input_pointer + PUSH_CHUNK_SIZE );
        buffer.PushSamples( temp_input );
        input_pointer += PUSH_CHUNK_SIZE;
        
        ASSERT_EQ( buffer.SpaceRemaining(), original_space + BUFFER_SIZE/2 - input_pointer );
    }
}

TEST_F( AudioBufferTest, CircularReading )
///
/// Checks that when reading from the buffer, samples are as you would expect.
/// In particular, after clearing samples and after the total number of added
/// samples is greater than the size of the buffer (with clearing samples in
/// between adds).
///
{
    // Constants
    const size_t BUFFER_SIZE = 44100*3;                                         // -> Size of the data cache in the audio buffer object.
    const size_t PUSH_CHUNK_SIZE = BUFFER_SIZE/10-3;                            // -> Number of samples per input call to the buffer object.
    const size_t NUM_CHUNKS_PER_TEST = BUFFER_SIZE/2.5/PUSH_CHUNK_SIZE;         // -> Number of chunks to push before checking the buffer contents each time.
    const size_t DEAD_SAMPLES_PER_TEST = 10;                                    // -> Number of samples to leave in the buffer for the next loop after clearing most of the buffer content.
    const int NUM_TESTS = BUFFER_SIZE/(PUSH_CHUNK_SIZE*NUM_CHUNKS_PER_TEST)*5;  // -> Number of times to fill and check the buffer content.
    
    // Create test object
    AudioBuffer<double> buffer = AudioBuffer<double>( BUFFER_SIZE );
    
    // Initialise with "Dead Samples" so that the number of old samples is consistent for each loop iteration below.
    std::vector<double> temp_input( this->input.begin(), this->input.begin() + DEAD_SAMPLES_PER_TEST );
    buffer.PushSamples( temp_input );
    size_t input_pointer = DEAD_SAMPLES_PER_TEST;
    
    // Loop through pushing in series of chunks, then checking the data, then clearing most of the data.
    for( int i=0; i<NUM_TESTS; ++i )
    {
        size_t original_input_position = input_pointer;
        
        // Add new samples
        for( int j=0; j<NUM_CHUNKS_PER_TEST; ++j )
        {
            std::vector<double> temp_input( this->input.begin() + input_pointer, this->input.begin() + input_pointer + PUSH_CHUNK_SIZE );
            buffer.PushSamples( temp_input );
            input_pointer += PUSH_CHUNK_SIZE;
        }
        
        // Check the samples in the buffer
        const double* the_data = buffer.Data();
        size_t num_data = buffer.NumSamples();
        for( int i=0; i<num_data; ++i )
        {
            ASSERT_EQ( the_data[i], this->input[i + original_input_position - DEAD_SAMPLES_PER_TEST] );
        }
        
        // Clear most of the buffer to make room for new samples.
        buffer.PopFront( buffer.NumSamples() - DEAD_SAMPLES_PER_TEST );
    }
}
