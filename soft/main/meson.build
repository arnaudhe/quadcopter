project('test_ahrs', 'cpp')

includes = include_directories('utils')
c_flags = ['-g', '-O0']

test_ahrs = executable('test_ahrs', 
                       ['utils/madgwick_ahrs.cpp', 
                        'utils/matrix.cpp', 
                        'utils/quaternion.cpp', 
                        'utils/vector.cpp', 
                        'utils/csv_reader.cpp', 
                        'tests/test_ahrs.cpp'], 
                       include_directories : includes, 
                       cpp_args : c_flags)
