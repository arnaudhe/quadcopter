project('test_ahrs', 'cpp')

includes = include_directories('.', '../../..')
c_flags = ['-g', '-O0']

test_ahrs = executable('test_ahrs', 
                       ['../../app/observers/euler_observer.cpp',
                        '../../utils/madgwick.cpp', 
                        '../../utils/matrix.cpp', 
                        '../../utils/quaternion.cpp', 
                        '../../utils/vector.cpp', 
                        '../../utils/csv_reader.cpp', 
                        'test_ahrs.cpp'], 
                       include_directories : includes, 
                       cpp_args : c_flags)

test_height = executable('test_height', 
                         ['../../app/observers/height_observer.cpp', 
                          '../../utils/kalman.cpp',
                          '../../utils/matrix.cpp', 
                          '../../utils/vector.cpp', 
                          '../../utils/csv_reader.cpp', 
                          'test_height.cpp'], 
                          include_directories : includes, 
                          cpp_args : c_flags)