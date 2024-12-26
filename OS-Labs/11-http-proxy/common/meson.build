common_sources = files(
  'hashmap.c',
  'buffer.c',
  'murmurhash.c'
)

common_static = static_library('common', 
  common_sources,
)

common = declare_dependency(
  include_directories: include_directories('.'),
  link_with: common_static
)

