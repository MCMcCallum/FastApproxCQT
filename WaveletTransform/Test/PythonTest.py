
import numpy as np
import matplotlib as mpl
mpl.rcParams['backend'] = 'TkAgg'
import matplotlib.pyplot as plt
from scipy import signal
import FastWavelet

winlen = 512
fftlen = winlen*8
window = np.hamming(winlen)
overlap = 1.0 - 1.0/(2.0**3)
overlapping_samples = winlen*overlap
sig_len = 44100/2
sin_freq = 2.0*np.pi/5.0
impulse_sample = sig_len/2

# fc = 0.99*np.exp(np.pi/(fftlen/winlen)*(1.0j));
# x = np.zeros(2049, dtype=complex)
# x[2049/2] = 1.0 + 0.0j;
# for ind in range(1, len(x)):
# 	x[ind] = fc*x[ind-1] + (1.0-fc)*x[ind]
# for ind in range(len(x)-2, 0, -1):
# 	x[ind] = fc*x[ind+1] + (1.0-fc)*x[ind]

# plt.figure(1)
# plt.subplot(211)
# plt.plot(np.real(x))
# plt.subplot(212)
# plt.plot(np.abs(np.fft.fft(x)))
# plt.show()

input_sig = np.random.normal(size=sig_len) + np.sin(sin_freq*np.arange(sig_len))
input_sig[impulse_sample] = 100*np.sqrt(np.mean(input_sig**2))

# answer = np.fft.fft(window*input_sig[0:256], 4096)
normal_spec = signal.spectrogram( input_sig, 
							   window=window, 
							   noverlap=int(overlapping_samples), 
							   nfft=4096, 
							   return_onesided=True, 
							   mode='complex' )
FWL = FastWavelet.FastWavelet(overlap, window)
result = FWL.PushSamples(input_sig)

coeffs = FWL.GetCQTCoeffs()
# plt.figure()
# plt.plot(np.abs(coeffs))
# plt.figure()
# plt.plot(np.angle(coeffs))
# plt.show()

fc = np.log(np.linspace(0.01, 100.0, len(normal_spec[2][:,0])))
fc = fc - np.min(fc)
fc = fc/np.max(fc)
fc = fc*np.exp(np.pi/(fftlen/winlen)*(1.0j))
spec = np.array(normal_spec[2]).T;
for frame in spec:
	for ind in range(1, len(frame)):
		frame[ind] = fc[ind-1]*frame[ind-1] + (1.0-fc[ind-1])*frame[ind]
	for ind in range(len(frame)-2, 0, -1):
		frame[ind] = fc[ind-1]*frame[ind+1] + (1.0-fc[ind-1])*frame[ind]

# plt.figure()
# plt.plot(np.abs(fc), 'r')
# plt.figure()
# plt.plot(np.angle(fc), 'r')
# plt.show()

# print repr(result)

plt.figure(1)
plt.subplot(211)
plt.imshow(np.log(np.abs(spec)).T, extent=[0,5,0,1.4])
plt.subplot(212)
plt.imshow(np.log(np.abs(result)).T, extent=[0,5,0,1.4])

plt.show()
