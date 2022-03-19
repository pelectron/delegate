project('delegate','cpp', version:'0.1', default_options:['cpp_std=c++17'])

delegate_dep = declare_dependency(include_directories:'include')
catch_dep = dependency('catch2', fallback:['catch2','catch2_dep'])

examples = [
executable( 'delegate_example', 
            sources:files('examples/delegate_example.cpp'), 
            include_directories:'include'),
executable( 'multicast_delegate_example', 
            sources:files('examples/multicast_delegate_example.cpp'),  
            include_directories:'include')]

test_sources = files( 'tests/delegate.t.cpp',
                      'tests/multicast_delegate.t.cpp',
                      'tests/r_value_ref_test.cpp',
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

doxygen = find_program('doxygen', 
                       ['C:/Program Files/doxygen/bin/doxygen.exe', 
                       get_option('doxy_exe')],
                       required:false)

if get_option('build_docs').enabled() and doxygen.found()
  # need to override the INPUT for doxygen when building with meson,
  # as meson does not guarantee running an external target in the source directory.
  # Furthermore, meson does not allow for direct shell invocation. A such, 
  # we have to invoke our shell command with a bat/bash script. These are 
  # located in the docs folder.
  # See https://www.doxygen.nl/manual/faq.html#faq_cmdline for details 
  # on the echo and piping for passing params to doxygen.
  source_dir  = meson.current_source_dir()  # the directory this meson.build file is located in
  include_dir = source_dir / 'include'      # absolute path to include directory of the project
  output_dir = source_dir           # absolute path to documentation output directory
  example_dir = source_dir / 'examples'     # absolute path to examples directory
  readme_file = source_dir / 'readme.md'    # absolute path to readme file
  #input files and directories
  input = 'INPUT=@0@'.format(' '.join(include_dir, example_dir, readme_file))
  #output directory
  output = 'OUTPUT_DIRECTORY=@0@'.format(output_dir)
  #example path
  example = 'EXAMPLE_PATH=@0@'.format(example_dir)
  # readme files
  readme = 'USE_MDFILE_AS_MAINPAGE=@0@'.format(readme_file)
  if host_machine.system() != 'windows'
    doxy_file = source_dir / 'Doxyfile'       # absolute path to doxygen config file  
    input = '"@0@"'.format(input)
    output = '"@0@"'.format(output)
    example = '"@0@"'.format(example)
    readme = '"@0@"'.format(readme)
    cmd = source_dir / 'docs' / 'build_docs.sh'
  else
    doxy_file = source_dir.replace('/','\\')  + '\Doxyfile' # absolute path to doxygen config file  
    cmd = source_dir / 'docs' / 'build_docs.bat'
  endif
  env = environment() # environment for docs target
  env.set('DOXYFILE', doxy_file)
  env.set('DOXYGEN_EXE', doxygen.full_path())
  env.set('DOXYGEN_OUTDIR', output)
  env.set('DOXYGEN_INPUT', input)
  env.set('DOXYGEN_EXAMPLE', example)
  env.set('DOXYGEN_README', readme)
  run_target('docs', command:[cmd], env:env, depends:examples)
endif