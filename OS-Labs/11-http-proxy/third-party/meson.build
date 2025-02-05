log_c_static = static_library('log-c', 'log.c')
log_c = declare_dependency(
  link_with: [log_c_static],
  include_directories: [include_directories('.')]
)

libcoro_static = static_library('libcoro', 'coro.c')
libcoro = declare_dependency(
  link_with: [libcoro_static],
  include_directories: [include_directories('.')]
)

picohttpparser_static = static_library('picohttpparser', 'picohttpparser.c')
picohttpparser = declare_dependency(
  link_with: [picohttpparser_static],
  include_directories: [include_directories('.')]
)
