
byte_array_sources = files([
    '../../utils/byte_array.cpp'
])

byte_array_includes = include_directories([
    '../../'
])

byte_array_dep = declare_dependency(
    sources : byte_array_sources,
    include_directories : byte_array_includes
)

test_byte_array = executable('test_byte_array',
                              sources : ['test_byte_array.cpp'],
                              dependencies : [asserts_dep, logs_mock_dep, byte_array_dep])
