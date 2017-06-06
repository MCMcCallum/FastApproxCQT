//
// Created by: Matthew McCallum
// 4th June 2017
//
// Class for efficient overlap-add operations in C++.
//

#ifndef CUPCAKE_OVERLAP_ADD_BUFFER_H
#define CUPCAKE_OVERLAP_ADD_BUFFER_H

// In module includes
#include "vector_functions.h"

// Thirdparty includes
// None.

// Std Lib includes
#include <vector>
#include <algorithm>
#include <assert.h>

namespace cupcake
{
    
template< typename T >
class OverlapAddBuffer
{
public:
    
    OverlapAddBuffer();
    OverlapAddBuffer( size_t size );
    ~OverlapAddBuffer();
    
    void PushSamples( const std::vector< T >& samples );
    void IncrementWritePosition( size_t increment);
    void PopFront( size_t numElements );
    void Read( std::vector< double >& output );
    
    const size_t NumSamples() const;
    const size_t SpaceRemaining() const;
    const size_t Size() const;
    
private:
    
    //
    // Data
    //
    std::vector< T > mData;
    
    //
    // Configuration
    //
    const size_t mBufferLength;
    
    //
    // Mechanics
    //
    size_t mReadHead;
    size_t mWriteHead;
    
    //
    // Constants
    //
    static const size_t DEFAULT_BUFFER_SIZE = 44100*10;
};

template< typename T >
OverlapAddBuffer< T >::OverlapAddBuffer() :
    mData( DEFAULT_BUFFER_SIZE+1 ),
    mBufferLength( DEFAULT_BUFFER_SIZE+1 ),
    mReadHead( 0 ),
    mWriteHead( 0 )
///
/// Default Constructor.
///
{
    
}

template< typename T >
OverlapAddBuffer< T >::OverlapAddBuffer( size_t size ) :
    mData( size+1 ),
    mBufferLength( size+1 ),
    mReadHead( 0 ),
    mWriteHead( 0 )
///
/// Constructor.
///
/// @param size
///  The maximum number of elements allowable in the buffer.
///
{
    
}

template< typename T >
OverlapAddBuffer< T >::~OverlapAddBuffer()
///
/// Destructor.
///
{
    
}

template< typename T >
void OverlapAddBuffer< T >::PushSamples( const std::vector< T >& samples )
///
/// Adds samples to the buffer by literally adding the values to any content that is
/// already written to the buffer ahead of the current write postion (zeros if none
/// previous written). This does not increment the write position. This is to be done
/// manually by the user of this class via IncrementWritePosition.
///
/// @param samples
///  A vector of samples to be added to the buffer.
///
{
    
    assert( samples.size() <= SpaceRemaining() ); // Buffer overflow if this condition is false.
    
    size_t samples_until_end = mBufferLength - mWriteHead;
    
    vec_add_in_place( samples.data(), mData.data() + mWriteHead, std::min( samples.size(), samples_until_end ) );
    
    if( samples.size() > samples_until_end )
    {
        vec_add_in_place( samples.data() + samples_until_end, mData.data(), samples.size() - samples_until_end );
    }
    
}

template< typename T >
void OverlapAddBuffer< T >::IncrementWritePosition( size_t increment )
///
/// Increments the position of the write head for the next push of samples to the buffer.
///
/// @param increment
///  The number of samples by which to increment the write position.
///
{
    
    assert( increment <= SpaceRemaining() ); // Buffer overflow if we want to read beyond this position by incrementing the write pointer.
    
    mWriteHead = ( mWriteHead + increment ) % mBufferLength;
    
}

template< typename T >
void OverlapAddBuffer< T >::PopFront( size_t numElements )
///
/// Removes a number of the oldest samples from the buffer. This also
/// zeros this section of the buffer in anticipation for future adds.
/// If the number of samples requested is more than or equal to that in the buffer,
/// the buffer is simply reset to zero.
///
/// @param numElements
///  The number of oldest elements to be removed from the buffer.
///
{
    
    if( numElements >= NumSamples() )
    {
        numElements = NumSamples();
    }
    
    vec_zero( mData.data() + mReadHead, std::min( mBufferLength - mReadHead, numElements ) );
    if( numElements > ( mBufferLength - mReadHead ) )
    {
        vec_zero( mData.data(), numElements - ( mBufferLength - mReadHead ) );
    }
    mReadHead = ( mReadHead + numElements ) % mBufferLength;
    
}
    
template< typename T >
void OverlapAddBuffer< T >::Read( std::vector< double >& output )
///
/// Copy a number of samples from the buffer into the provided output vector.
/// These samples are not removed from the buffer, they are simply copied to the
/// output.
///
/// @param output
///  A vector of length according to the number of desired samples to be read. It
///  will be filled with the output.size() number of oldest samples in the buffer.
///
{
    
    assert( output.size() <= NumSamples() );
    
    vec_copy( mData.data() + mReadHead, output.data(), std::min( mBufferLength - mReadHead, output.size() ) );
    
    if( output.size() > ( mBufferLength - mReadHead ) )
    {
        vec_copy( mData.data(), output.data() + ( mBufferLength - mReadHead ), output.size() - ( mBufferLength - mReadHead ) );
    }
    
}

template< typename T >
const size_t OverlapAddBuffer< T >::NumSamples() const
///
/// Returns the number of samples remaining in the buffer.
///
/// @return
///  The number of samples remaining in the buffer.
///
{
    
    size_t num_samples = mWriteHead >= mReadHead ? mWriteHead - mReadHead : mWriteHead + mBufferLength - mReadHead;
    return num_samples;
    
}

template< typename T >
const size_t OverlapAddBuffer< T >::SpaceRemaining() const
///
/// Returns the number of samples beyond which will cause a buffer overflow.
///
/// @return
///  The number of samples that can be added before buffer overflow occurs.
///
{
    
    return mBufferLength - 1 - NumSamples();
    
}

template< typename T >
const size_t OverlapAddBuffer< T >::Size() const
///
/// Returns the total number of allowable samples in the buffer. Should be equal to
/// SpaceRemaining() + NumSamples().
///
/// @return
///  The total number of samples that can fit in the buffer.
///
{
    
    return mBufferLength - 1;
    
}
    
} // namespace cupcake

#endif // CUPCAKE_OVERLAP_ADD_BUFFER_H
