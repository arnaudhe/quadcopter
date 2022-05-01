
observers_sources = files([
    '../../app/observers/euler_observer.cpp',
    '../../app/observers/height_observer.cpp',
    '../../utils/madgwick.cpp',
    '../../utils/kalman.cpp',
    '../../utils/matrix.cpp',
    '../../utils/quaternion.cpp',
    '../../utils/vector.cpp',
    '../../utils/csv_reader.cpp'
])

observers_includes = include_directories([
    '../../'
])

observers_dep = declare_dependency(
    sources : observers_sources,
    include_directories : observers_includes
)

test_ahrs = executable('test_ahrs',
                       sources: ['test_ahrs.cpp'],
                       dependencies : [espressif_mock_dep, os_mock_dep, logs_mock_dep, observers_dep])

test_height = executable('test_height',
                         sources: ['test_height.cpp'],
                         dependencies : [espressif_mock_dep, os_mock_dep, logs_mock_dep, observers_dep])
