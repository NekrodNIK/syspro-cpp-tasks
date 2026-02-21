project(
  'cpp-utils', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++23']
)

tests = [
  'tests/test_avl.cpp',
  'tests/test_plane.cpp'
]

gtest_dep = dependency('gtest', main : true)
e = executable('testprog', tests, dependencies : gtest_dep)
test('gtest test', e)

