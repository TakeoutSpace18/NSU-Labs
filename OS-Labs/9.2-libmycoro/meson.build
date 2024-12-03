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

test('tst-many-coros', 
  executable('tst-many-coros',
    ['tst-many-coros.c'],
    link_with: libmycoro
  )
)
