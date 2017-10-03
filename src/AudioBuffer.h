//
// Created by: Matthew McCallum
// 4th June 2017
//
// FIFO buffer for audio - could be used in the future to wrap a circular buffer.
//

#ifndef CUPCAKE_AUDIO_BUFFER_H
#define CUPCAKE_AUDIO_BUFFER_H

// In module includes
// None.

// Thirdparty includes
// None.

// Std Lib includes
#include <vector>
#include <algorithm>
#include <assert.h>

namespace cupcake
{

template< typename T >
class AudioBuffer
{
    
public:

	AudioBuffer();
	AudioBuffer( size_t size );
	~AudioBuffer();

	void PushSamples( const std::vector< T >& samples );
	void PopFront( size_t numElements );

	const size_t NumSamples() const;
	const size_t SpaceRemaining() const;
    const size_t Size() const;
    
    const T* Data();

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
AudioBuffer<T>::AudioBuffer() :
	mData( 2*(DEFAULT_BUFFER_SIZE+1) ),
	mBufferLength( DEFAULT_BUFFER_SIZE+1 ),
	mReadHead( 0 ),
	mWriteHead( 0 )
///
/// Default Constructor.
///
{
    
}

template< typename T >
AudioBuffer<T>::AudioBuffer( size_t size ) :
	mData( 2*(size+1) ),
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
AudioBuffer<T>::~AudioBuffer()
///
/// Destructor.
///
{
    
}

template< typename T >
void AudioBuffer<T>::PushSamples( const std::vector< T >& samples )
///
/// Copy samples into the AudioBuffer's memory. It is on the user of the class
/// to ensure the buffer doesn't overflow.
/// This copies the samples twice so that when reading from the buffer
/// overlaps the end of the circular buffer, we can still get a contiguous
/// block of memory with as many of the buffer samples as we like, without
/// having to rearrange the memory.
///
/// @param samples
///  A vector of samples to be added to the AudioBuffer.
///
{

    assert( samples.size() <= SpaceRemaining() ); // Buffer overflow if this condition is false.

    size_t samples_until_end = mBufferLength - mWriteHead;

	memcpy( mData.data() + mWriteHead, samples.data(), std::min( samples.size(), samples_until_end )*sizeof( T ) );
	memcpy( mData.data() + mWriteHead + mBufferLength, samples.data(), std::min( samples.size(), samples_until_end )*sizeof( T ) );

	if( samples.size() > samples_until_end )
	{
		memcpy( mData.data(), samples.data() + samples_until_end, ( samples.size() - samples_until_end )*sizeof( T ) );
		memcpy( mData.data() + mBufferLength, samples.data() + samples_until_end, ( samples.size() - samples_until_end )*sizeof( T ) );
	}

	mWriteHead = ( mWriteHead + samples.size() ) % mBufferLength;

}

template< typename T >
void AudioBuffer<T>::PopFront( size_t numElements )
///
/// Clear a number of (the oldest) samples from the beginning of the buffer. It is
/// on the user of the class to ensure they do not clear more samples than are in the
/// buffer. This frees up room for new samples to be added to the buffer.
/// If the number of samples requested is more than or equal to that in the buffer, 
/// the buffer is simply reset to start reading and writing at its first index.
///
/// @param numElements
///  The number of values to erase from the start of the buffer.
///
{

	if( numElements >= NumSamples() )
	{
		mReadHead = 0;
		mWriteHead = 0;
	}
	else
	{
		mReadHead = ( mReadHead + numElements ) % mBufferLength;
	}
	
}

template< typename T >
const size_t AudioBuffer<T>::NumSamples() const
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
const size_t AudioBuffer<T>::SpaceRemaining() const
///
/// Returns the number of samples beyond which will cause a buffer overflow.
///
/// @return
///  The number of samples that can be added before buffer overflow occurs.
///
{

	return mBufferLength - 1 - NumSamples(); // minus 1 because mReadHead==mWriteHead cannot be in the same place unless the buffer is empty.

}
    
template< typename T >
const size_t AudioBuffer<T>::Size() const
///
/// Returns the maximum number of elements in total that can be written to an empty buffer.
///
/// @return
///  The number of elements in the buffer.
///
{
    return mBufferLength - 1;
}

template< typename T >
const T* AudioBuffer<T>::Data()
///
/// Getter function returning a pointer to the first element of the contiguous
/// data in the buffer.
///
/// @return
///  A pointer to the first element in the buffer.
///
{
    return mData.data() + mReadHead;
}

} // namespace cupcake

#endif // CUPCAKE_AUDIO_BUFFER_H
