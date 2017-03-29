  {
    'includes': [
      '../../../build/common.gypi',
    ],
    'targets': [
      {
        'target_name': 'WaveletTransformLib',
        'type': 'shared_library',
        'include_dirs': [
          '.',
          'include',
        ],
        'sources': [
          'src/AudioBuffer.h',
          'src/FastWavelet.h',
          'src/FastWavelet.cpp',
          'src/FastWaveletPythonBinding.cpp',
          'src/PybindArgumentConversion.h',
          'src/STFT.h',
          'src/VectorFunctions.h',
        ],
        'link_settings': {
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/Accelerate.framework',
            '$(SDKROOT)/System/Library/Frameworks/Python.framework',
            '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
          ],
        },
        'conditions': [
          ['OS!="win"', {
            'xcode_settings' : {
              'OTHER_CPLUSPLUSFLAGS' : [ # @todo [matt.mcc 03/25/17] All the below flags are required for python compilation... We need to sort through these...
              '-fno-strict-aliasing',
              '-fno-common',
              '-dynamic',
              '-g',
              '-Os',
              '-pipe', 
              '-fno-common',
              '-fno-strict-aliasing',
              '-fwrapv',
              '-DENABLE_DTRACE',
              '-DMACOSX',
              '-DNDEBUG',
              '-Wall',
              '-Wstrict-prototypes',
              '-Wshorten-64-to-32',
              '-DNDEBUG',
              '-g',
              '-fwrapv',
              '-Os',
              '-Wall',
              '-Wstrict-prototypes',
              '-DENABLE_DTRACE', 
              ],
            }
          }],
        ],
      },
    ],
  }