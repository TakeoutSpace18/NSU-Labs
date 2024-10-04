project('tcp_file_transfer', 'c',
  version : '0.0.1',
  default_options: ['warning_level=3']
)

inc = include_directories('.')

common_src = files('log.c', 'protocol.c')

common_lib = static_library(
  'common',
  common_src,
  )

subdir('server')
subdir('client')
