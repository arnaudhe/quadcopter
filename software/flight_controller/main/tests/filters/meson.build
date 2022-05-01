
filters_sources = files([
    '../../utils/filter.cpp'
])

filters_includes = include_directories([
    '../../'
])

filters_dep = declare_dependency(
    sources : filters_sources,
    include_directories : filters_includes
)

test_low_pass = executable('test_low_pass',
                          sources : ['test_low_pass.cpp'],
                          dependencies : [espressif_mock_dep, logs_mock_dep, filters_dep])

test_slew = executable('test_slew',
                        sources : ['test_slew.cpp'],
                        dependencies : [espressif_mock_dep, logs_mock_dep, filters_dep])
