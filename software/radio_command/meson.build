project('radio_command', ['c', 'cpp'],
        version: '1.0.0',
        default_options: ['werror=true',
                          'buildtype=debug'])

jpeg_lib = meson.get_compiler('c').find_library('jpeg', dirs : [join_paths(meson.current_source_dir(), 'toolchain/sysroot/usr/lib')])

jpeg_dep = declare_dependency(
    include_directories : include_directories('toolchain/sysroot/usr/include'),
    dependencies : jpeg_lib
)

pthread_dep = declare_dependency(
    link_args : ['-lpthread'],
    compile_args : ['-D_REENTRANT']
)

executable('radio_command',
           include_directories: include_directories('.'),
           sources: ['main.cpp',
                     'app/radio_command.cpp',
                     'app/video_feedback.cpp',
                     'periph/i2c.cpp',
                     'periph/spi.cpp',
                     'periph/gpio.cpp',
                     'periph/adc.cpp',
                     'drv/SI4432.cpp',
                     'drv/ADS1115.cpp',
                     'hal/button.cpp',
                     'hal/stick.cpp',
                     'hal/battery.cpp',
                     'hal/radio.cpp',
                     'hal/video_capture.cpp',
                     'hal/framebuffer.cpp',
                     'os/task.cpp',
                     'utils/byte_array.cpp',
                     'utils/crc16.cpp',
                     'utils/jpeg_decoder.cpp',
                     'utils/pixmap.cpp'],
           dependencies : [jpeg_dep, pthread_dep])
