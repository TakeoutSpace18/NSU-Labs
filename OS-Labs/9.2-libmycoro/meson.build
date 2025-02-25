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

test('tst-recursive', 
  executable('tst-recursive',
    ['tst-recursive.c'],
    link_with: libmycoro
  )
)

test('tst-normal', 
  executable('tst-normal',
    ['tst-normal.c'],
    link_with: libmycoro
  )
)
