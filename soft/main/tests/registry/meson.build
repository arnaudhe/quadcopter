
registry_sources = files([
    '../../data_model/data_ressource.cpp',
    '../../data_model/data_ressources_registry.cpp',
    '../../data_model/json_protocol.cpp',
    '../../data_model/binary_protocol.cpp',
    '../../data_model/ressource_enum.cpp'
])

registry_includes = include_directories([
    '../../'
])

registry_dep = declare_dependency(
    sources : registry_sources,
    include_directories : registry_includes
)

test_registry = executable('test_json',
                            sources : ['test_json.cpp'],
                            dependencies : [espressif_mock_dep, os_mock_dep, logs_mock_dep, byte_array_dep, registry_dep])

test_binary = executable('test_binary',
                          sources : ['test_binary.cpp'],
                          dependencies : [espressif_mock_dep, os_mock_dep, logs_mock_dep, byte_array_dep, registry_dep])
