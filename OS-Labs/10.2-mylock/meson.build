project('mylock', 'c')

mymutex_static = static_library('mymutex', ['mymutex.c'])

mymutex = declare_dependency(
  link_with: mymutex_static,
)

myspinlock_static = static_library('myspinlock', ['myspinlock.c'])

myspinlock = declare_dependency(
  link_with: myspinlock_static,
)

executable('tst-locks',
  ['tst-locks.c'],
  dependencies: [myspinlock, mymutex]
)
