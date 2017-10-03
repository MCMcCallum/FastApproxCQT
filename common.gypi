{
  'variables': {
    'base_dir': '.',
    'thirdparty_lib_dir': '<(base_dir)/thirdparty/lib/',
    'thirdparty_include_dir': '<(base_dir)/thirdparty/include/',
  },
  'target_defaults' : 
  {
    'include_dirs': 
    [
      '<(thirdparty_include_dir)',
    ],

    'link_settings': {
      'libraries': [
        '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
      ],
    },

    'xcode_settings' : 
    {
      'GCC_SYMBOLS_PRIVATE_EXTERN'        : 'YES',
      'SYMROOT'                           : './bin',
      'GCC_WARN_ABOUT_RETURN_TYPE'        : 'YES',
      'GCC_WARN_NON_VIRTUAL_DESTRUCTOR'   : 'YES',
      'GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS' : 'YES',
      'GCC_WARN_PEDANTIC'                 : 'YES',
      'GCC_WARN_UNUSED_VARIABLE'          : 'YES',
      'GCC_WARN_UNUSED_FUNCTION'          : 'YES',
      'OTHER_CPLUSPLUSFLAGS'              : 
      [ 
        "-std=c++14",

        # Flags below required for python.
        '-fno-strict-aliasing',
        '-fno-common',
        '-dynamic',
        '-g',
        '-Os',
        '-pipe', # Not required?
        '-fwrapv',
        '-Wall',              # Warnings
        '-Wstrict-prototypes',# Warnings
        '-Wshorten-64-to-32', # Warnings
      ],
      'CLANG_CXX_LANGUAGE_STANDARD'       : 'c++14',
      'CLANG_CXX_LIBRARY'                 : 'libc++',
    }
  }
}
