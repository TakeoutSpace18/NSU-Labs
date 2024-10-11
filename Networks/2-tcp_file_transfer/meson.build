project('tcp_file_transfer', 'c',
  version : '0.0.1',
  default_options: ['warning_level=3']
)

add_project_arguments('-DLOG_USE_COLOR', language : 'c')

inc = include_directories('.', 'third-party/')

common_src = files(
  'third-party/log.c',
  'third-party/progressbar.c',
  'common.c')

curses_dep = dependency('curses')

common_lib = static_library(
  'common',
  common_src,
  dependencies : curses_dep
  )

subdir('server')
subdir('client')
