project(
  'cpp-utils', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20']
)

gtest_dep = dependency('gtest', main : true)
e = executable('testprog', 'tests/test_avl.cc', dependencies : gtest_dep)
test('gtest test', e)
