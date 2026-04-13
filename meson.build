project(
  'cpp-utils', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20']
)

tests = [
  'tests/test_avl.cpp',
  'tests/test_plane.cpp',
  'tests/test_matrix.cpp',
  'tests/test_instance_limiter.cpp',
]

gtest_dep = dependency('gtest', main : true)
e = executable('testprog', tests, dependencies : gtest_dep)
test('gtest test', e)

