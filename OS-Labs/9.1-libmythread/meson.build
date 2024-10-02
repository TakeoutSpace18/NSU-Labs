project('libmythread', 'c',
  default_options: ['warning_level=3']
  )

libmythread_src = files(
  'libmythread.c',
  )

libmythread = library(
  'mythread',
  libmythread_src
  )

# incdir = include_directories()
test1 = executable('test1',
  ['test.c', 'libmythread.h'],
  # include_directories : incdir,
  link_with: libmythread
  )

test('test1', test1)
