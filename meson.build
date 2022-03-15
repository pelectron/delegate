project('delegate','cpp', version:'0.1', default_options:['cpp_std=c++17'])

delegate_dep = declare_dependency(include_directories:'include')
catch_dep = dependency('catch2', fallback:['catch2','catch2_dep'], required:false)
example = executable('delegate_example', files('examples/delegate_example.cpp'), include_directories:'include')
multi_example = executable('multicast_delegate_example',files('examples/multicast_delegate.cpp'),  include_directories:'include')
if catch_dep.found()
test_sources = files('tests/delegate.t.cpp', 'tests/multicast_delegate.t.cpp', 'tests/r_value_ref_test.cpp', 'tests/test_main.cpp')
  test_main = executable('test_main', test_sources, include_directories:'include', dependencies:[catch_dep])
endif