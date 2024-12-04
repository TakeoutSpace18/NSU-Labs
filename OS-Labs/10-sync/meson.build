project('os-sync', 'c', version: '0.0.1', default_options: ['warning_level=3'])

executable('queue-no-sync',
  ['queue-threads.c', 'queue.c']
)

executable(
  'queue-spinlock',
  ['queue-threads.c', 'queue-spinlock.c'],
  c_args: ['-DUSE_SPINLOCK'],
)

executable('queue-mutex',
  ['queue-threads.c', 'queue-mutex.c'],
  c_args: ['-DUSE_MUTEX']
)

executable(
  'queue-condvar',
  ['queue-threads.c', 'queue-condvar.c'],
  c_args: ['-DUSE_MUTEX', '-DUSE_CONDVAR'],
)

executable(
  'queue-semaphore',
  ['queue-threads.c', 'queue-semaphore.c'],
  c_args: ['-DUSE_SEMAPHORE', '-DUSE_MUTEX'],
)
