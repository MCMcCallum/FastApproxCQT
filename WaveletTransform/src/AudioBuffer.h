//
// Created: 03/25/17 by Matthew McCallum
//
// FIFO buffer for audio - could be used in the future to wrap a circular buffer.
//

#ifndef CUPCAKE_AUDIO_BUFFER_H
#define CUPCAKE_AUDIO_BUFFER_H

#include <vector>
#include <assert.h>

namespace cupcake
{
    
template< typename T >
class AudioBuffer
{
    
public:
    
    AudioBuffer( size_t size );
    
    void AddSamples( const std::vector<T>& samples );
    
    void RemoveSamples( size_t num_samples );
    
    size_t NumSamples();
    
    size_t Size();
    
    const T* Data();
    
    // Index Operator
    
    // Iterator
    
private:
    
    // Parameters
    
    // Mechanics
    size_t mWritePointer;
    
    // Data
    std::vector<T> mBuffer;
    
};
    
template< typename  T >
AudioBuffer<T>::AudioBuffer( size_t size ) :
mWritePointer( 0 ),
mBuffer( size, 0 )
///
/// Constructor.
///
{
    
}

template< typename  T >
void AudioBuffer<T>::AddSamples( const std::vector<T>& samples )
///
/// Copy samples into the AudioBuffer's memory. It is on the user of the class
/// to ensure the buffer doesn't overflow.
///
/// @param samples
///  A vector of samples to be added to the AudioBuffer.
///
{

    assert( samples.size() < ( mBuffer.size() - mWritePointer ) ); // Buffer overflow.
    
    memcpy( mBuffer.data() + mWritePointer, samples.data(), sizeof( T )*samples.size() );
    mWritePointer += samples.size();
    
}

template< typename  T >
void AudioBuffer<T>::RemoveSamples( size_t num_samples )
///
/// Clear a number of (the oldest) samples from the beginning of the buffer. It is
/// on the user of the class to ensure they do not clear more samples than are in the
/// buffer. This frees up room for new samples to be added to the buffer.
///
/// @param num_samples
///  The number of values to erase from the start of the buffer.
///
{
    
    assert( num_samples <= write_pointer ); // Tried to erase more samples than exist in buffer.
    
    memmove( mBuffer.data(), mBuffer.data() + num_samples, sizeof( T )*( mWritePointer - num_samples ) );
    mWritePointer -= num_samples;
    
}
    
template< typename T >
size_t AudioBuffer<T>::NumSamples()
///
/// Returns the number of samples currently in the buffer.
///
/// @return
///  The number of samples currently in the buffer.
///
{
    return mWritePointer;
}
    
template< typename T >
size_t AudioBuffer<T>::Size()
///
/// Returns the capacity of the buffer, i.e., the maximum number of samples that fit
/// inside the buffer.
///
/// @return
///  The buffer's capacity.
///
{
    return mBuffer.size();
}
    
template< typename T >
const T* AudioBuffer<T>::Data()
///
/// Returns a pointer to the first (oldest) element in the buffer memory. This is
/// useful for fast operations (without copy) that operate on the buffer memory.
///
/// @return
///  A pointer to the first (oldest) element in the buffer.
///
{
    return mBuffer.data();
}
    
} // namespace cupcake

#endif // CUPCAKE_AUDIO_BUFFER_H