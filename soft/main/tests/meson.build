project('unit_tests', ['c', 'cpp'], version : '1.0', default_options : ['buildtype=debug'])

asserts_dep = declare_dependency(
    include_directories : include_directories(['.'])
)

espressif_mock_dep = declare_dependency(
    sources: ['mock/espressif/esp_err.c',
              'mock/espressif/esp_spiffs.c'],
    compile_args: ['-DSOURCE_DIR="' + meson.current_source_dir() + '"'],
    include_directories : include_directories(['mock/espressif'])
)

os_mock_dep = declare_dependency(
    sources : ['mock/app/os/mutex.cpp'],
    include_directories : include_directories(['mock/app'])
)

logs_mock_dep = declare_dependency(
    include_directories : include_directories(['mock/app'])
)

subdir('byte_array')
subdir('filters')
subdir('registry')
subdir('observers')
