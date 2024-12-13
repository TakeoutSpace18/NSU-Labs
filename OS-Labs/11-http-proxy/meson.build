project('http-proxy', 'c',
  version : '0.0.1',
  default_options: ['warning_level=3']
)

toplevel_include = include_directories('.')

libcares = dependency('libcares')

add_project_arguments('-DLOG_USE_COLOR', language : 'c')
# add_project_arguments('-DCORO_DEBUG', language : 'c')

subdir('third-party')
subdir('common')
subdir('server')

executable('http-proxy', [
    'main.c',
    'proxy.c',
    'http_request.c',
  ],
  include_directories : toplevel_include,
  dependencies : [libcares, common, server, log_c]
)
