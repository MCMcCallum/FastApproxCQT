
import numpy as np
import matplotlib as mpl
mpl.rcParams['backend'] = 'TkAgg'
import matplotlib.pyplot as plt
from scipy import signal
import FastWavelet



class SlowWavelet( object ):
	"""
	A Python class for computing a wavelet like transform on a
	signal, but it operates very slow.
	"""

	def __init__( self, fft_len, window, overlap ):
		"""
		Constructor.

		Arg fft_len:
			Type: int
			Value: The length of the FFT operation in the STFT

		Arg window:
			Type: numpy.ndarray - 1D
			Value: A vector of float values containing the windowing
				   function for the STFT operation.

		Arg overlap:
			Type: float
			Value: The fraction of the length of window that overlaps
			       between successive STFT frames.
		"""
		self._win_len = window.size
		self._fft_len = fft_len
		self._window = window
		self._fc = self._compute_coeffs( self._win_len, self._fft_len )
		self._overlapping_samples = int( self._win_len*overlap )

	def PushSamples( self, input_sig ):
		"""
		Pushes samples to be processed via this slow wavelet-like
		transform.

		Arg input_sig:
			Type: numpy.ndarray - 1D
			Value: A vector of float samples to be transformed via this
				   transform.

		Return:
			Type: numpy.ndarray - 2D complex
			Value: A 2D array containing complex valued samples as a result
				   of this transform. Frequency is along the first dimension
				   and time is along the second.

			Type: numpy.ndarray - 1D
			Value: The time indices of the start of each STFT window in samples.

			Type: numpy.ndarray - 1D
			Value: The frequency values of each frequency bin in radians per sample.
		"""
		normal_spec = signal.spectrogram( input_sig, 
									      window=self._window, 
									      noverlap=self._overlapping_samples, 
									      nfft=self._fft_len, 
									      return_onesided=True, 
									      mode='complex' )

		spec = np.array( normal_spec[2] ).T;
		for frame in spec:
			for ind in range( 1, len( frame ) ):
				frame[ind] = self._fc[ind-1]*frame[ind-1] + ( 1.0-self._fc[ind-1] )*frame[ind]
			for ind in range( len(frame)-2, 0, -1 ):
				frame[ind] = self._fc[ind-1]*frame[ind+1] + ( 1.0-self._fc[ind-1] )*frame[ind]

		return spec

	def GetTimeAndFreqIndices( self, spec_size ):
		"""
		Gets the time and frequency indices for a previous output of
		the PushSamples function.

		Arg spec_size:
			Type: (int, int)
			Value: The shape of the previous output. The first element corresponds
				   to the number of frequency bins, and the second to the number
				   of time windows.

		Return:
			Type: numpy.ndarray - 1D
			Value: The time indices of the start of each STFT window in samples.

			Type: numpy.ndarray - 1D
			Value: The frequency values of each frequency bin in radians per sample.
		"""
		time_inds = np.arange( spec_size[1] )*( self._win_len - self._overlapping_samples )
		freq_inds = np.linspace( 0, np.pi, spec_size[0], endpoint=False )
		return time_inds, freq_inds

	def _compute_coeffs( self, win_len, fft_len ):
		"""
		Helper function for calculating IIR coefficients for this transform.

		Arg win_len:
			Type: int
			Value: The length of the STFT window used in this class.

		Arg fft_len:
			Type: int
			Value: The length of the FFT used in the STFT operation.

		Return:
			Type: numpy.ndarray - 1D complex
			Value: The coefficients to be used to adaptively filter
			       an STFT across frequency in the frequency domain.
		"""
		fc = np.log( np.linspace( 0.01, 100.0, fft_len/2 ) )
		fc = fc - np.min( fc )
		fc = fc/np.max( fc )
		fc = fc*np.exp( np.pi/( fft_len/win_len )*( 1.0j ) )
		return fc
	


def Demo():
	"""
	A simple demo to plot the result of a fast and slow CQT and output
	the respective times that it took to compute them.
	"""
	# Parameters
	win_len = 512
	fft_len = win_len*8
	window = np.hamming( win_len )
	overlap = 1.0 - 1.0/( 2.0**3 )

	# Create input Signal
	sig_len = 44100/2
	sin_freq = 2.0*np.pi/5.0
	impulse_sample = sig_len/2
	input_sig = np.random.normal( size=sig_len ) + np.sin( sin_freq*np.arange( sig_len ) )
	input_sig[impulse_sample] = 100*np.sqrt( np.mean( input_sig**2 ) )

	# Create wavelet objects
	slow_alg = SlowWavelet( fft_len, window, overlap )
	fast_alg = FastWavelet.FastWavelet( overlap, window )

	# Process signal
	print( "Processing Slow Wavelet..." )
	slow_result = slow_alg.PushSamples( input_sig )
	print( "Processing Fast Wavelet..." )
	fast_result = fast_alg.PushSamples( input_sig )

	# Display result
	time_ind, freq_ind = slow_alg.GetTimeAndFreqIndices( slow_result.T.shape )
	min_time = np.min( time_ind )
	max_time = np.max( time_ind )
	min_freq = np.min( freq_ind )
	max_freq = np.max( freq_ind )
	plt.figure(1)
	plt.subplot(211)
	plt.imshow( np.log( np.abs( slow_result ) ).T, 
				extent=[min_time,max_time,min_freq,max_freq], 
				aspect='auto',
				origin='lower' )
	plt.ylabel( "Frequency (rad/sample)" )
	plt.title( "Result from slow Python Wavelet" )
	plt.subplot(212)
	plt.imshow( np.log( np.abs( fast_result ) ).T, 
				extent=[min_time,max_time,min_freq,max_freq], 
				aspect='auto',
				origin='lower' )
	plt.xlabel( "Time (samples)" )
	plt.ylabel( "Frequency (rad/sample)" )
	plt.title( "Result from fast C++ Wavelet" )
	plt.show()



if __name__=='__main__':
	Demo()
