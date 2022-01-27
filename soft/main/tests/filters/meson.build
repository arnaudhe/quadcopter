project('test_filters', 'cpp')

test_filters = executable('test_low_pass',
                          sources : ['../../utils/filter.cpp',
                                     'test_low_pass.cpp'],
                          include_directories : include_directories('.', '../../..'),
                          cpp_args : ['-g', '-O0'])

test_filters = executable('test_slew',
                          sources : ['../../utils/filter.cpp',
                                     'test_slew.cpp'],
                          include_directories : include_directories('.', '../../..'),
                          cpp_args : ['-g', '-O0'])