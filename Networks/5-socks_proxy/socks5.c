#include "socks5.h"

#include <ares.h>

#include "c.h"
#include "dns.h"
#include "log.h"
#include "server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFSIZE 4096

/* called on failed recv, send */
static inline void attribute_noreturn() io_error(int status)
{
    if (status == 0) {
        client_log_error("IO Error: Connection closed");
    }
    else if (status == -1) {
        client_log_error("IO Error: %s", strerror(errno));
    }
    else {
        client_log_error("IO Error: Unknown status");

    }

    client_drop();
    unreachable();
}

static void send_cmd_response(struct cmd_response *response)
{
    ssize_t ret;
    size_t size;
    size_t domain_name_len;

    size = offsetof(struct cmd_response, bnd_addr);
    ret = client_send_buf(client_sfd(), response, size);
    if (ret == -1 || ret == 0)
        io_error(ret);

    if (response->atyp == IPV4) {
        ret = client_send_buf(client_sfd(), &response->bnd_addr.ipv4, 4);
        if (ret == -1 || ret == 0)
            io_error(ret);
    }
    else if (response->atyp == DOMAINNAME) {
        domain_name_len = strlen(response->bnd_addr.domain_name);
        ret = client_send_buf(client_sfd(), &response->bnd_addr.domain_name,
                              domain_name_len);
        if (ret == -1 || ret == 0)
            io_error(ret);
    }
    else if (response->atyp == IPV6) {
        ret = client_send_buf(client_sfd(), &response->bnd_addr.ipv6, 16);
        if (ret == -1 || ret == 0)
            io_error(ret);
    }
    else {
        client_log_error("Unsupported destination address type");
        client_drop();
    }

    ret = client_send_buf(client_sfd(), &response->bnd_port, sizeof(response->bnd_port));
    if (ret == -1 || ret == 0)
        io_error(ret);
}

static inline void attribute_noreturn() command_error(uint8_t err)
{
    struct cmd_response response = { 0 };
    response.ver = SOCKS_VERSION5;
    response.rep = err;
    response.atyp = IPV4;

    send_cmd_response(&response);
    client_drop();

    unreachable();
}

static void negotiate_auth_method(void)
{
    struct auth_method_selection_request request;
    struct auth_method_selection_response response;
    ssize_t ret;
    size_t size;
    
    size = offsetof(struct auth_method_selection_request, methods);
    ret = client_recv_buf(client_sfd(), &request, size);
    if (ret == -1 || ret == 0)
        io_error(ret);

    ret = client_recv_buf(client_sfd(), &request.methods, request.nmethods);
    if (ret == -1 || ret == 0)
        io_error(ret);

    client_log_trace("Received auth methods list message");

    if (request.ver != SOCKS_VERSION5) {
        log_error("Wrong client protocol version");
        client_drop();
    }

    response.ver = SOCKS_VERSION5;
    response.method = NOMETHODS;

    for (uint8_t i = 0; i < request.nmethods; ++i) {
        if (request.methods[i] == NOAUTH) {
            response.method = NOAUTH;
        }
    }

    if (response.method == NOMETHODS) {
        log_error("Client doesn't support authentication absence");
        client_drop();
    }

    ret = client_send_buf(client_sfd(), &response, sizeof(response));
    if (ret == -1 || ret == 0)
        io_error(ret);

    client_log_trace("Sent auth method confirmation message");
}

/* returns malloced string that needs to be freed */
static char *receive_domain_name(void)
{
    ssize_t ret;
    uint8_t len;
    char *name = NULL;

    ret = client_recv_buf(client_sfd(), &len, sizeof(len));
    if (ret == -1 || ret == 0)
        goto io_fail;

    name = (char *) malloc(len + 1);
    if (!name) {
        log_error("Out of memory");
        client_drop();
    }

    ret = client_recv_buf(client_sfd(), name, len);
    if (ret == -1 || ret == 0)
        goto io_fail;


    name[len] = '\0';

    return name;

io_fail:
    free(name);
    io_error(ret);
    return NULL;
}

static void receive_cmd_request(struct cmd_request *request)
{
    ssize_t ret;
    size_t size;

    memset(request, 0, sizeof(*request));

    size = offsetof(struct cmd_request, dst_addr);
    ret = client_recv_buf(client_sfd(), request, size);
    if (ret == -1 || ret == 0)
        goto io_fail;

    client_log_trace("Receiving command...");

    if (request->ver != SOCKS_VERSION5) {
        client_log_error("Wrong client protocol version");
        client_drop();
    }

    if (request->atyp == IPV4) {
        ret = client_recv_buf(client_sfd(), &request->dst_addr.ipv4, 4);
        if (ret == -1 || ret == 0)
            goto io_fail;
        
        client_log_trace("Received ipv4 address");

    }
    else if (request->atyp == DOMAINNAME) {
        request->dst_addr.domain_name = receive_domain_name();
        client_log_trace("Received domain name address");
    }
    else if (request->atyp == IPV6) {
        ret = client_recv_buf(client_sfd(), &request->dst_addr.ipv6, 6);
        if (ret == -1 || ret == 0)
            goto io_fail;

        client_log_trace("Received ipv6 address");
    }
    else {
        client_log_error("Unsupported address type");
        command_error(ADDRESS_TYPE_NOT_SUPPORTED);
    }

    ret = client_recv_buf(client_sfd(), &request->dst_port, sizeof(request->dst_port));
    if (ret == -1 || ret == 0)
        goto io_fail;

    client_log_trace("Received destination port");

    return;

io_fail:
    if (request->atyp == DOMAINNAME)
        free(request->dst_addr.domain_name);

    io_error(ret);
}

static int create_host_socket(void)
{
    int sfd = -1;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        client_log_error("Failed to create host socket: %s", strerror(errno));
        command_error(SERVER_FAILURE);
    }

    return sfd;
}

static int domain_name_connect(const char *domain, uint16_t port) {
    struct ares_addrinfo *result = NULL;

    struct ares_addrinfo_hints hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    char port_str[16 + 1];
    snprintf(port_str, 16, "%i", ntohs(port));

    result = client_dns_resolve(domain, port_str, &hints);
    if (!result) {
        client_log_error("Domain name resolution failed");
        command_error(HOST_UNREACHABLE);
    }

    int sfd;
    struct ares_addrinfo_node *node;
    for (node = result->nodes; node != NULL; node = node->ai_next) {
        sfd = create_host_socket();

        if (connect(sfd, node->ai_addr, node->ai_addrlen) == 0) {
            break;
        }
        client_log_debug("connect: %s", strerror(errno));

        close(sfd);
    }

    if (node == NULL) {
        client_log_error("Failed to connect to host %s", domain);
        command_error(CONN_REFUSED);
    }

    ares_freeaddrinfo(result);
    return sfd;
}

static int ipv4_connect(uint32_t ipv4, uint16_t port)
{
    struct sockaddr_in sa = { 0 };

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = ipv4;
    sa.sin_port = port;

    int sfd = create_host_socket();

    if (connect(sfd, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
        client_log_error("Failed to connect to host: %s", strerror(errno));
        close(sfd);
        command_error(CONN_REFUSED);
    }

    return sfd;
}

static int cmd_connect(const struct cmd_request *cmd)
{
    int sfd = -1;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        client_log_error("Failed to create host socket: %s", strerror(errno));
        command_error(SERVER_FAILURE);
    }

    if (cmd->atyp == DOMAINNAME) {
        sfd = domain_name_connect(cmd->dst_addr.domain_name, cmd->dst_port);
    }
    else if (cmd->atyp == IPV4) {
        sfd = ipv4_connect((uint32_t) *cmd->dst_addr.ipv4, cmd->dst_port);
    }
    else if (cmd->atyp == IPV6) {
        command_error(ADDRESS_TYPE_NOT_SUPPORTED);
    }

    client_log_info("Connected to destination host");

    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    if (getsockname(sfd, (struct sockaddr *) &server, &server_len) != 0) {
        client_log_error("getsockname() failed: %s", strerror(errno));
        client_drop();
    }

    struct cmd_response response = { 0 };
    response.ver = SOCKS_VERSION5;
    response.rep = SUCCEEDED;
    response.atyp = IPV4;
    *response.bnd_addr.ipv4 = server.sin_addr.s_addr;
    response.bnd_port = server.sin_port;

    send_cmd_response(&response);

    return sfd;
}

void attribute_noreturn() socks5_main(void)
{
    struct cmd_request cmd_request;
    int host_sfd = -1;

    negotiate_auth_method();
    receive_cmd_request(&cmd_request);

    if (cmd_request.cmd == CONNECT) {
        host_sfd = cmd_connect(&cmd_request);
    } 
    else if (cmd_request.cmd == UDP_ASSOCIATE) {
        client_log_error("UPD ASSOCIATE command is not supported");
        command_error(COMMAND_NOT_SUPPORTED);
    }
    else if (cmd_request.cmd == BIND) {
        client_log_error("BIND command is not supported");
        command_error(COMMAND_NOT_SUPPORTED);
    }
    else {
        client_log_error("unknown command");
        command_error(COMMAND_NOT_SUPPORTED);
    }

    char client_buf[BUFSIZE];
    char host_buf[BUFSIZE];
    ssize_t client_read, host_read;

    client_watch_fd(host_sfd);
    for (;;) {
        client_read = client_recv_nonblock(client_sfd(), client_buf, BUFSIZE, 0);
        if (client_read == -1) {
            client_log_error("Failed to read client socket");
            io_error(-1);
        }

        host_read = client_recv_nonblock(host_sfd, host_buf, BUFSIZE, 0);
        if (host_read == -1) {
            client_log_error("Failed to read host socket");
            io_error(-1);
        }

        if (client_read > 0) {
            client_log_trace("Read %zi bytes from client", client_read);
            client_send_buf(host_sfd, client_buf, client_read);
        }

        if (host_read > 0) {
            client_log_trace("Read %zi bytes from host", host_read);
            client_send_buf(client_sfd(), host_buf, host_read);
        }

        if (client_read == 0 || host_read == 0) {
            client_drop();
        }

        if (client_read == NODATA || host_read == NODATA) {
            client_yield();
        }
    }

    unreachable();
}
