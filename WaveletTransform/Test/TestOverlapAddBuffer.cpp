//
// Created by: Matthew McCallum
// 4th June 2017
//
// Test class for OverlapAddBuffer class
//

// In module includes
#include "OverlapAddBuffer.h"
#include "sig_gen.h"

// Thirdparty includes
#include "gtest/gtest.h"

// Std Lib includes
#include <vector>
#include <functional>
#include <algorithm>

using namespace cupcake;

class OverlapAddBufferTest : public ::testing::Test
///
/// Test fixture for overlap-add buffer tests.
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
        input_noise.resize( MAX_INPUT_SIZE );
        std::generate( input_noise.begin(), input_noise.end(), std::bind( &make_random_number, -1.0, 1.0 ) );
        
        input_dc.resize( 0 );
        input_dc.resize( MAX_INPUT_SIZE, make_random_number( -1.0, 1.0 ) );
    }
    
    std::vector< double > input_noise; // Vectors for storing the input for all tests.
    std::vector< double > input_dc;
    
};

TEST_F( OverlapAddBufferTest, test_write_read )
///
/// Tests that after writing a chunk to the buffer and incrementing the write position
/// we are able to read that same chunk several times.
///
{
    
    const size_t INPUT_CHUNK_SIZE = 125;                            // -> The size of each chunk of samples input to the buffer.
    const size_t BUFFER_SIZE = 933;                                 // -> The size of the buffer we are processing samples through.
    const size_t NUM_CHUNKS_INPUT = BUFFER_SIZE/INPUT_CHUNK_SIZE*10;// -> The number of times to run this test - pushing chunks into the buffer and reading.
    
    OverlapAddBuffer< double > buffer( BUFFER_SIZE );
    
    size_t input_pos = 0;
    for( int chunk=0; chunk<NUM_CHUNKS_INPUT; ++chunk )
    {
        const std::vector< double > input( input_noise.begin() + input_pos, input_noise.begin() + input_pos + INPUT_CHUNK_SIZE );
        std::vector< double > output1( INPUT_CHUNK_SIZE );
        std::vector< double > output2( INPUT_CHUNK_SIZE );
        std::vector< double > output3( INPUT_CHUNK_SIZE );
        
        buffer.PushSamples( input );
        buffer.IncrementWritePosition( INPUT_CHUNK_SIZE );
        
        buffer.Read( output1 );
        buffer.Read( output2 );
        buffer.Read( output3 );
        
        for( int samp_ind=0; samp_ind<INPUT_CHUNK_SIZE; ++samp_ind )
        {
            EXPECT_EQ( output1[samp_ind], input[samp_ind] );
            EXPECT_EQ( output2[samp_ind], input[samp_ind] );
            EXPECT_EQ( output3[samp_ind], input[samp_ind] );
        }
        
        buffer.PopFront( INPUT_CHUNK_SIZE );
        
        input_pos += INPUT_CHUNK_SIZE;
    }
    
}

TEST_F( OverlapAddBufferTest, test_write_with_overlap )
///
/// Tests After writing several overlapping chunks into the buffer, that the overlapping chunks
/// are added together correctly.
///
{
    
    const size_t INCREMENT_SIZE = 25;                               // -> The number of samples between each write position in the buffer
    const size_t NUM_OVERLAPPING = 5;                               // -> The number of overlapping chunks at any point in the buffer (after the wind-up/wind-down)
    const size_t INPUT_CHUNK_SIZE = NUM_OVERLAPPING*INCREMENT_SIZE; // -> The size of a chunk pushed into the buffer
    const size_t BUFFER_SIZE = 933;                                 // -> The number of samples in the buffer itself
    const size_t NUM_CHUNKS_INPUT = BUFFER_SIZE/INCREMENT_SIZE*10;  // -> The number of chunks input into the buffer for this test
    
    OverlapAddBuffer< double > buffer( BUFFER_SIZE );
    
    size_t input_pos = 0;
    for( size_t chunk=0; chunk<NUM_CHUNKS_INPUT; ++chunk )
    {
        const std::vector< double > input( input_dc.begin() + input_pos, input_dc.begin() + input_pos + INPUT_CHUNK_SIZE );
        std::vector< double > output( INCREMENT_SIZE );
        
        buffer.PushSamples( input );
        buffer.IncrementWritePosition( INCREMENT_SIZE );
        
        buffer.Read( output );
        
        double expected_level = std::min( chunk + 1, NUM_OVERLAPPING )*input_dc[0];
        for( int samp_ind=0; samp_ind<INCREMENT_SIZE; ++samp_ind )
        {
            EXPECT_EQ( output[samp_ind], expected_level );
        }
        
        buffer.PopFront( INCREMENT_SIZE );
        
        input_pos += INCREMENT_SIZE;
    }
    
}

TEST_F( OverlapAddBufferTest, test_space_remaining )
///
/// Checks that the space reamining reported by the buffer is correct after reading/writing.
///
{
    
    const size_t EXTRA_SAMPLE_PUSH_SIZE = 100;  // -> An additional amount of samples to push into the buffer between each full-buffer write/clear cycle
    const size_t NUM_EXTRA_PUSHES = 10;         // -> The number of times the additional amount of samples are pushed in between each full-buffer write/clear cycle
    const size_t BUFFER_SIZE = 1045;            // -> The size of the buffer itself
    const size_t NUM_CYCLES = 10;               // -> The number of times we fill/clear the buffer then write additional samples
    const size_t PUSH_CHUNK_SIZE = 110;         // -> The maximum size of the chunks pushed into the buffer when filling it up
    
    OverlapAddBuffer< double > buffer( BUFFER_SIZE );
    
    for( int cycle=0; cycle<NUM_CYCLES; ++cycle )
    {
        int num_chunks = 0;
        while( buffer.SpaceRemaining() )
        {
            EXPECT_EQ( buffer.SpaceRemaining(), BUFFER_SIZE - num_chunks*PUSH_CHUNK_SIZE );
            
            size_t input_size = std::min( PUSH_CHUNK_SIZE, buffer.SpaceRemaining() );
            
            const std::vector< double > input( input_dc.begin(), input_dc.begin() + input_size );
            
            buffer.PushSamples( input );
            buffer.IncrementWritePosition( input_size );
            
            ++num_chunks;
        }
        
        EXPECT_EQ( buffer.SpaceRemaining(), 0 );
        
        buffer.PopFront( BUFFER_SIZE );
        
        EXPECT_EQ( buffer.SpaceRemaining(), BUFFER_SIZE );
        
        const std::vector< double > input( input_dc.begin(), input_dc.begin() + EXTRA_SAMPLE_PUSH_SIZE );
        
        for( int extra_push=0; extra_push<NUM_EXTRA_PUSHES; ++extra_push )
        {
            buffer.PushSamples( input );
            
            EXPECT_EQ( buffer.SpaceRemaining(), BUFFER_SIZE );
        }
        
        buffer.IncrementWritePosition( EXTRA_SAMPLE_PUSH_SIZE );
        
        EXPECT_EQ( buffer.SpaceRemaining(), BUFFER_SIZE - EXTRA_SAMPLE_PUSH_SIZE );
        
        buffer.PopFront( EXTRA_SAMPLE_PUSH_SIZE );
    }
    
}

TEST_F( OverlapAddBufferTest, test_num_samples )
///
/// Test that the buffer reports the correct number of samples after writing to/clearing the buffer
///
{
    const size_t PUSH_CHUNK_SIZE = 100;                                                 // -> The number of samples to write to the buffer each time
    const size_t PUSHES_PER_CYCLE = 3;                                                  // -> The number of times to push samples to the buffer before clearing it
    const size_t READ_SIZE = 33;                                                        // -> The maximum number of samples to pop from the buffer each time whilst clearing it
    const size_t BUFFER_SIZE = 933;                                                     // -> The size of the buffer itself
    const size_t NUM_READ_CYCLES = BUFFER_SIZE/( PUSH_CHUNK_SIZE*PUSHES_PER_CYCLE )*10; // -> The number of times to cycle through writing/popping samples to/from the buffer.
    
    OverlapAddBuffer< double > buffer( BUFFER_SIZE );
    
    for( int cycle=0; cycle<NUM_READ_CYCLES; ++cycle )
    {
        
        for( int push=0; push<PUSHES_PER_CYCLE; push++ )
        {
            const std::vector< double > input( input_dc.begin(), input_dc.begin() + PUSH_CHUNK_SIZE );
            
            buffer.PushSamples( input );
            buffer.IncrementWritePosition( input.size() );
        }
        
        int num_reads = 0;
        while( buffer.NumSamples() )
        {
            EXPECT_EQ( buffer.NumSamples(), PUSH_CHUNK_SIZE*PUSHES_PER_CYCLE - num_reads*READ_SIZE );
            
            size_t current_read_size = std::min( READ_SIZE, buffer.NumSamples() );
            
            buffer.PopFront( current_read_size );
            
            ++num_reads;
        }
        
        EXPECT_EQ( buffer.NumSamples(), 0 );
        
    }
    
}
