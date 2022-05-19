project('hello_world', ['c', 'cpp'],
        version: '1.0.0',
        default_options: ['werror=true',
                          'buildtype=debug'])

executable('radio_command',
           sources: ['main.cpp',
                     'i2c.cpp',
                     'spi.cpp',
                     'gpio.cpp'])
