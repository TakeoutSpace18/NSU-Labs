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

test('tst-create-join', 
  executable('tst-create-join',
    ['tst-create-join.c'],
    link_with: libmythread
  )
)

test('tst-cancel', 
  executable('tst-cancel',
    ['tst-cancel.c'],
    link_with: libmythread
  )
)

test('tst-many-threads', 
  executable('tst-many-threads',
    ['tst-many-threads.c'],
    link_with: libmythread
  )
)
