#include "socks5.h"

#include "c.h"
#include "log.h"
#include "server.h"
#include "socket.h"
#include <string.h>


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
    ret = client_send_buf(&response, size);
    if (ret == -1 || ret == 0)
        io_error(ret);

    if (response->atyp == IPV4) {
        ret = client_send_buf(&response->bnd_addr.ipv4, 4);
        if (ret == -1 || ret == 0)
            io_error(ret);
    }
    else if (response->atyp == DOMAINNAME) {
        domain_name_len = strlen(response->bnd_addr.domain_name);
        ret = client_send_buf(&response->bnd_addr.domain_name, domain_name_len);
        if (ret == -1 || ret == 0)
            io_error(ret);
    }
    else if (response->atyp == IPV6) {
        ret = client_send_buf(&response->bnd_addr.ipv6, 16);
        if (ret == -1 || ret == 0)
            io_error(ret);
    }
    else {
        client_log_error("Unsupported destination address type");
        client_drop();
    }

    ret = client_send_buf(&response->bnd_port, sizeof(response->bnd_port));
    if (ret == -1 || ret == 0)
        io_error(ret);
}

static inline void attribute_noreturn() command_error(uint8_t err)
{
    struct cmd_response response;
    memset(&response, 0, sizeof(response));
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
    ret = client_recv_buf(&request, size);
    if (ret == -1 || ret == 0)
        io_error(ret);

    ret = client_recv_buf(&request.methods, request.nmethods);
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

    ret = client_send_buf(&response, sizeof(response));
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

    ret = client_recv_buf(&len, sizeof(len));
    if (ret == -1 || ret == 0)
        goto io_fail;

    name = (char *) malloc(len + 1);
    if (!name) {
        log_error("Out of memory");
        client_drop();
    }

    ret = client_recv_buf(name, len);
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
    ret = client_recv_buf(request, size);
    if (ret == -1 || ret == 0)
        goto io_fail;

    client_log_trace("Receiving command...");

    if (request->ver != SOCKS_VERSION5) {
        client_log_error("Wrong client protocol version");
        client_drop();
    }

    if (request->atyp == IPV4) {
        ret = client_recv_buf(&request->dst_addr.ipv4, 4);
        if (ret == -1 || ret == 0)
            goto io_fail;
        
        client_log_trace("Received ipv4 address");

    }
    else if (request->atyp == DOMAINNAME) {
        request->dst_addr.domain_name = receive_domain_name();
        client_log_trace("Received domain name address");
    }
    else if (request->atyp == IPV6) {
        ret = client_recv_buf(&request->dst_addr.ipv6, 6);
        if (ret == -1 || ret == 0)
            goto io_fail;

        client_log_trace("Received ipv6 address");
    }
    else {
        client_log_error("Unsupported address type");
        command_error(ADDRESS_TYPE_NOT_SUPPORTED);
    }

    ret = client_recv_buf(&request->dst_port, sizeof(request->dst_port));
    if (ret == -1 || ret == 0)
        goto io_fail;

    client_log_trace("Received destination port");

io_fail:
    if (request->atyp == DOMAINNAME)
        free(request->dst_addr.domain_name);

    io_error(ret);
}

void attribute_noreturn() socks5_main(void)
{
    struct cmd_request cmd_request;

    negotiate_auth_method();
    receive_cmd_request(&cmd_request);

    client_log_info("Received command");
    if (cmd_request.atyp == DOMAINNAME) {
        client_log_info("domain name: %s", cmd_request.dst_addr.domain_name);
    }

    unreachable();
}
