project('delegate','cpp', version:'0.1', default_options:['cpp_std=c++17'])

delegate_dep = declare_dependency(include_directories:'include')
catch_dep = dependency('catch2', fallback:['catch2','catch2_dep'])
all_library_sources = files('examples/delegate_example.cpp', 'examples/multicast_delegate_example.cpp', 'include/delegate.hpp', 'include/multicast_delegate.hpp')
examples = [
executable( 'delegate_example', 
            sources:files('examples/delegate_example.cpp'), 
            include_directories:'include'),
executable( 'multicast_delegate_example', 
            sources:files('examples/multicast_delegate_example.cpp'),  
            include_directories:'include')]

test_sources = files( 'tests/delegate.t.cpp',
                      'tests/multicast_delegate.t.cpp',
                      'tests/test_main.cpp')

test_debug = executable('test_debug', 
                        sources:test_sources, 
                        include_directories:'include', 
                        dependencies:[catch_dep])

test_release = executable('test_release', 
                          sources:test_sources, 
                          include_directories:'include', 
                          dependencies:[catch_dep],
                          override_options:['buildtype=release'])

test('delegate_test', test_debug)
test('release_build_test', test_release)

if get_option('build_docs').enabled()
  # doxygen executable
  doxygen = find_program('doxygen', ['C:/Program Files/doxygen/bin/doxygen.exe', get_option('doxygen_path')], required:true)
  # doxybook2 executable
  doxybook = find_program('doxybook2', [get_option('doxybook_path')], required:true)
  # python executable
  python   = find_program('python', [get_option('python_path')], required:true)
  # the directory this meson.build file is located in
  source_dir  = meson.current_source_dir() 
  # path to documentation build script
  build_script = source_dir / 'build_docs.py'
  docs = custom_target('docs', command:[python, build_script, 
                                        '--doxygen-path', doxygen.full_path(), 
                                        '--doxybook-path', doxybook.full_path(), 
                                        '--root', source_dir,
                                        '--templates', source_dir / 'doxybook' / 'templates',
                                        '--config', source_dir / 'doxybook'/'config.json',
                                        '--htmldir', source_dir / 'docs',
                                        '--readme', source_dir / 'readme.in.md',
                                        '--generate-github-readme', '--verbose'],
                                        depend_files: [all_library_sources],
                                        input:all_library_sources,
                                        output:'docs_target')
  endif