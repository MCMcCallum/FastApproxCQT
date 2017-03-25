#
# Empirical Mode Decomposition
#

import numpy as np
import matplotlib.pyplot as plt
import pyeemd
import argparse
import wave
import struct

class EMDAnalyser:

	def __init__( self, filename ):
		self.f = wave.open( filename, 'r' )
		self.numChannels = self.f.getnchannels( )
		self.sampRate = self.f.getframerate( )

	def __del__( self ):
		self.f.close( )

	def Process( self, num_seconds ):
		data = self.f.readframes( num_seconds*self.sampRate )
		samples = struct.unpack('h'*num_seconds*self.sampRate*self.numChannels, data)
		self.leftSamples = np.array( samples[::self.numChannels] )
		self.rightSamples = np.array( samples[1::self.numChannels] )
		divisor = 2.0**16/2
		self.leftSamples = self.leftSamples.astype( float )/divisor
		self.rightSamples = self.rightSamples.astype( float )/divisor
		imfs = pyeemd.eemd( self.leftSamples, ensemble_size=100, noise_strength=0.2 )
		print imfs.shape
		plt.subplot(8,1,1)
		plt.plot(self.leftSamples)
		plt.xlim(7000,8500)
		for i in range(2, 7):
			plt.subplot(8,1,i)
			plt.plot(imfs[i-2])
			plt.xlim(7000,8500)
		plt.subplot(8,1,8)

		plt.show()


if (__name__ == '__main__'):
	
	parser = argparse.ArgumentParser(description='Plot EMD for an audio file and compare to STFT.')
	parser.add_argument( '-f', default='/Users/mattmccallum/Data/LDC93S1.wav' )
	args = parser.parse_args( )
	filename = args.f

	anal = EMDAnalyser( filename )
	anal.Process( 2 )

