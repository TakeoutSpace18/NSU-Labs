project('socks_proxy', 'c',
  version : '0.0.1',
  default_options: ['warning_level=3']
)

add_project_arguments('-DLOG_USE_COLOR', language : 'c')
# add_project_arguments('-DCORO_DEBUG', language : 'c')

inc = include_directories('.', 'third-party/')

# libev_dep = dependency('libev')
libcares_dep = dependency('libcares')

socks_proxy = executable('socks-proxy', [
    'main.c',
    'server.c',
    'dns.c',
    'socks5.c',
    'third-party/coro.c',
    'third-party/log.c',
  ],
  include_directories : inc,
  link_args: '-lev',
  dependencies : [libcares_dep]
)
