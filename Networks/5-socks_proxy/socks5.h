#ifndef SOCKS5_H
#define SOCKS5_H

#include "server.h"
#include "c.h"

#define SOCKS_VERSION5 0x05
    
struct auth_method_selection_request {
    uint8_t ver;
    uint8_t nmethods;
    uint8_t methods[256];
} attribute_packed();

struct auth_method_selection_response {
    uint8_t ver;
    uint8_t method;
} attribute_packed();

enum auth_method {
    NOAUTH = 0x00,
    GSSAPI = 0x01,
    USERPASS = 0x02,
    NOMETHODS = 0xff
};

enum cmd {
    CONNECT = 0x01,
    BIND = 0x02,
    UDP_ASSOCIATE = 0x03
};

enum address_type {
    IPV4 = 0x01,
    DOMAINNAME = 0x03,
    IPV6 = 0x04
};

enum cmd_response_type {
    SUCCEEDED = 0x00,
    SERVER_FAILURE = 0x01, /* general SOCKS server failure */
    CONN_NOT_ALLOWED = 0x02, /* connection not allowed by ruleset */
    NETWORK_UNREACHEBLE = 0x03,
    HOST_UNREACHABLE = 0x04,
    CONN_REFUSED = 0x05, /* Connection refused */
    TTL_EXPIRED = 0x06,
    COMMAND_NOT_SUPPORTED = 0x07,
    ADDRESS_TYPE_NOT_SUPPORTED = 0x08
};

struct cmd_request {
    uint8_t ver;
    uint8_t cmd;
    uint8_t rsv;
    uint8_t atyp; /* address type */
    union {
        uint8_t ipv4[4];
        char *domain_name;
        uint8_t ipv6[16];
    } dst_addr;
    uint16_t dst_port;
} attribute_packed();

struct cmd_response {
    uint8_t ver;
    uint8_t rep;
    uint8_t rsv;
    uint8_t atyp;
    union {
        uint8_t ipv4[4];
        char *domain_name;
        uint8_t ipv6[16];
    } bnd_addr;
    uint16_t bnd_port;
} attribute_packed();

void socks5_main(void);

#endif /* !SOCKS5_H */

