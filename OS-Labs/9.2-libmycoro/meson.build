project('libmycoro', 'c',
  default_options: ['warning_level=3']
  )

libmycoro_src = files(
  'libmycoro.c',
  )

libmycoro = library(
  'mycoro',
  libmycoro_src
  )

test('tst-create-join', 
  executable('tst-create-join',
    ['tst-create-join.c'],
    link_with: libmycoro
  )
)

test('tst-cancel', 
  executable('tst-cancel',
    ['tst-cancel.c'],
    link_with: libmycoro
  )
)

test('tst-many-coros', 
  executable('tst-many-coros',
    ['tst-many-coros.c'],
    link_with: libmycoro
  )
)
