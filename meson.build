project('delegate','cpp', version:'0.1', default_options:['cpp_std=c++17'])

delegate_dep = declare_dependency(include_directories:'include')
catch_dep = dependency('catch2', fallback:['catch2','catch2_dep'], required:false)
example = executable('delegate_example', files('examples/delegate_example.cpp'), include_directories:'include')
if catch_dep.found()
  delegate_test = executable('delegate_test', files('tests/delegate.t.cpp'), include_directories:'include', dependencies:catch_dep)
endif