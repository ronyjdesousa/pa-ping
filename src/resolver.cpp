#include "resolver.h"
#include "net_compat.h"

int resolver_c::Resolve(pcc_t hostname, host_c &host)
{
    addrinfo hints;
    addrinfo *result = NULL;

    if (net_compat_c::Init() != SUCCESS)
        return ERROR_SOCKET_GENERALFAILURE;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int error = getaddrinfo(hostname, NULL, &hints, &result);

    net_compat_c::Cleanup();

    if (error != 0 || result == NULL)
        return ERROR_SOCKET_CANNOTRESOLVE;

    memset(&host.Address, 0, sizeof(host.Address));
    memcpy(&host.Address, result->ai_addr, result->ai_addrlen);

    host.AddressLength = result->ai_addrlen;
    host.AddressFamily = result->ai_family;
    host.Hostname = hostname;

    in_addr ipv4Address;
    in6_addr ipv6Address;

    host.HostIsIP =
        inet_pton(AF_INET, hostname, &ipv4Address) == 1 ||
        inet_pton(AF_INET6, hostname, &ipv6Address) == 1;

    freeaddrinfo(result);

    return SUCCESS;
}
