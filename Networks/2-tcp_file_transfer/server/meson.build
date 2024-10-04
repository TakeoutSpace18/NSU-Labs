receiver_server_src = files(
    'receiver_server.c',
    'client_thread.c',
)

receiver_server_exe = executable(
    'receiver-server',
    receiver_server_src,
    link_with : common_lib,
    include_directories : inc
)
