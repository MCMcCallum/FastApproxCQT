
import numpy as np
import FastWavelet

window = np.hamming(256)
FWL = FastWavelet.FastWavelet(256, 128, window)
input = np.random.rand(300)
answer = np.sum(input[0:256])
result = FWL.PushSamples(input)
print "The Answer Is: " + repr(answer)
print "We Got: " + repr(result)