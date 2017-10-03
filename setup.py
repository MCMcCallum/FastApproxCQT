from distutils.core import setup, Extension
import os

# @todo [matt.mccallum 10.02.17] Repeating all the parameters that exist in the gyp
#                                configurations here is really cumbersome.
#                                I should create a script that creates python distutils
#                                files from gyp configurations.

root_dir = os.path.dirname(os.path.realpath(__file__))

sources = [os.path.join( 'src', 'FastWavelet.cpp' ),
           os.path.join( 'src', 'FastWaveletPythonBinding.cpp' ),
           os.path.join( 'VecLib', 'src', 'FFT.cpp' ),
           os.path.join( 'VecLib', 'src', 'sig_gen.cpp' ),
           os.path.join( 'VecLib', 'src', 'vector_functions.cpp' )]

include_dirs = [os.path.join( 'src' ),
                os.path.join( 'VecLib', '' ),
                os.path.join( 'pybind11', 'include' ),
                os.path.join( 'VecLib', 'thirdparty', 'include' )]

libraries = [os.path.join( root_dir, 'VecLib', 'thirdparty', 'lib', 'libippcore.a' ),
             os.path.join( root_dir, 'VecLib', 'thirdparty', 'lib', 'libipps.a' ),
             os.path.join( root_dir, 'VecLib', 'thirdparty', 'lib', 'libippvm.a' )]

library_dirs = [os.path.join( root_dir, 'VecLib', 'thirdparty', 'lib', '' )]

extra_compile_args = ['-std=c++14',
                      '-Wno-deprecated-register',
                      '-Wno-deprecated-declarations', 
                      '-U__STRICT_ANSI__',  
                      '-fno-strict-aliasing',
                      '-fno-common',
                      '-dynamic',
                      '-g',
                      '-Os',
                      '-pipe',
                      '-fwrapv',
                      '-Wall',              
                      '-Wstrict-prototypes',
                      '-Wshorten-64-to-32']

extra_link_args = ['-framework', 'Python', 
                   os.path.join( root_dir, 'VecLib', 'thirdparty', 'lib', 'libippcore.a' ),
                   os.path.join( root_dir, 'VecLib', 'thirdparty', 'lib', 'libipps.a' ),
                   os.path.join( root_dir, 'VecLib', 'thirdparty', 'lib', 'libippvm.a' )]

FastWavelet = Extension( 'FastWavelet',
                         include_dirs = include_dirs,
                         libraries = libraries,
                         library_dirs = library_dirs,
                         sources = sources,
                         extra_compile_args = extra_compile_args,
                         extra_link_args = extra_link_args )

setup ( name = 'FastWavelet',
        version = '0.0',
        description = 'A module for computing a wavelet like transform super fast!',
        author = 'Matt C. McCallum',
        author_email = 'matt.c.mccallum@gmail.com',
        url = 'http://github.com/MCMcCallum/FastWavelet',
        long_description = '''

            A module for computing a wavelet like transform super fast!

            Methods:

                FastWavelet( window )
                    Arg window:
                        A 1D numpy array containing the windowing function used for STFT
                        analysis.
                    Return:
                        An object used to compute the Fast Wavelet transform here

                FastWavelet.PushSamples( samples )
                    Arg samples:
                        A 1D numpy array containing audio samples for which to take the
                        Fast Wavelet transform.
                    Return:
                        A 2D complex numpy array containing the output of the Fast Wavelet
                        transform of all input samples (plus any internally buffered state).

                FastWavelet.GetWindow()
                    Return:
                        A 1D numpy array containing the windowing function used for STFT analysis.

                FastWavelet.GetCQTCoeffs()
                    Return:
                        A 1D complex numpy array containing the IIR filter coefficients used for 
                        smoothing across frequency to allow adaptive windowing length.

        ''',
        ext_modules = [FastWavelet] )
