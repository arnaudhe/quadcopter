project('test_registry', 'cpp')

includes = include_directories(['.', '../..'])
c_flags = ['-g', '-O0']

test_registry = executable('test_registry', 
                            ['../../data_model/data_ressource.cpp', 
                             '../../data_model/data_ressources_registry.cpp', 
                             '../../data_model/json_protocol.cpp', 
                             '../../data_model/ressource_enum.cpp', 
                             'hal/udp_server.cpp',
                             'test_registry.cpp'], 
                            include_directories : includes, 
                            cpp_args : c_flags)
