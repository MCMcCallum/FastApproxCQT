  {
    'includes': 
    [
      '../../../build/common.gypi',
      '../../../VecLib/VecLib.gypi',
    ],
    'targets': 
    [
      {
        'target_name': 'WaveletTransformLib',
        'type': 'shared_library',
        'include_dirs': 
        [
          './src',
        ],

        'sources': 
        [
          'src/AudioBuffer.h',
          'src/CQT.h',
          'src/FastWavelet.h',
          'src/FastWavelet.cpp',
          'src/FastWaveletPythonBinding.cpp',
          'src/PybindArgumentConversion.h',

          'src/OverlapAddBuffer.h',
          'src/STFTAnalysis.h',
          'src/STFTSynthesis.h',
        ],

        'link_settings': 
        {
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/Python.framework',
          ],
        },
      },

      {
        'target_name': 'test',
        'type': 'executable',

        'include_dirs': 
        [
          './src',
        ],

        'sources': 
        [
          'src/AudioBuffer.h',
          'src/CQT.h',
          'src/FastWavelet.h',
          'src/FastWavelet.cpp',
          'src/FastWaveletPythonBinding.cpp',
          'src/PybindArgumentConversion.h',

          'src/OverlapAddBuffer.h',
          'src/STFTAnalysis.h',
          'src/STFTSynthesis.h',

          'test/TestAudioBuffer.cpp',
          'test/TestOverlapAddBuffer.cpp',
          'test/TestSTFTAnalysis.cpp',
          'test/TestSTFTAnalysisSynthesis.cpp',
          'test/TestSTFTSynthesis.cpp',
        ],

        'link_settings': 
        {
          'libraries': 
          [
            '$(SDKROOT)/System/Library/Frameworks/Python.framework',
            '<(thirdparty_lib_dir)/libgtest.a',
            '<(thirdparty_lib_dir)/libgtest_main.a',
          ],
        },
      },
    ],
  }


