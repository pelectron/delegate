project('delegate','cpp', version:'0.1', default_options:['cpp_std=c++17'])

delegate_dep = declare_dependency(include_directories:'include')
catch_dep = dependency('catch2', fallback:['catch2','catch2_dep'])

executable( 'delegate_example', 
            sources:files('examples/delegate_example.cpp'), 
            include_directories:'include')

executable( 'multicast_delegate_example', 
            sources:files('examples/multicast_delegate_example.cpp'),  
            include_directories:'include')

test_sources = files( 'tests/delegate.t.cpp',
                      'tests/multicast_delegate.t.cpp',
                      'tests/r_value_ref_test.cpp',
                      'tests/test_main.cpp')

test_main = executable('test_main', 
                        sources:test_sources, 
                        include_directories:'include', 
                        dependencies:[catch_dep])

test_release = executable('test_release', 
                          sources:test_sources, 
                          include_directories:'include', 
                          dependencies:[catch_dep],
                          override_options:['buildtype=release'])

test('delegate_test', test_main)
test('release_build_test', test_release)

if get_option('build_docs').enabled()
  run_target('docs', command:[get_option('doxy_dir_path') / 'doxygen', meson.current_source_dir()/'Doxyfile'])
endif