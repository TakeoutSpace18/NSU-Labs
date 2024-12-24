server_source = files(
  'server.c',
  'worker_thread.c',
  'client_context.c',
  'coroutine.c',
  'dns.c'
)

server_static = static_library('server',
  server_source,
  dependencies: [libcoro, log_c, libcares, common],
  include_directories: [toplevel_include]
)

server = declare_dependency(
  link_with: [server_static],
  link_args: '-lev',
  include_directories: include_directories('.')
)
