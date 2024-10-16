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

tst_create_join = executable('tst-create-join',
  ['tst-create-join.c'],
  # include_directories : incdir,
  link_with: libmythread
  )

test('tst-create-join', tst_create_join)

tst_cancel = executable('tst-cancel',
  ['tst-cancel.c'],
  # include_directories : incdir,
  link_with: libmythread
  )

test('tst-cancel', tst_cancel)
