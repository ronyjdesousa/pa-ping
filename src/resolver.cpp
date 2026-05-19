#include "resolver.h"
#include "net_compat.h"

int resolver_c::Resolve(pcc_t hostname, host_c &host)
{
    hostent* remoteHost = NULL;

    if (net_compat_c::Init() != SUCCESS)
        return ERROR_SOCKET_GENERALFAILURE;

    remoteHost = gethostbyname(hostname);

    net_compat_c::Cleanup();

    if (remoteHost == NULL)
        return ERROR_SOCKET_CANNOTRESOLVE;

    sockaddr_in address;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr = *(in_addr*)remoteHost->h_addr_list[0];

    memset(&host.Address, 0, sizeof(host.Address));
    memcpy(&host.Address, &address, sizeof(address));

    host.AddressLength = sizeof(address);
    host.AddressFamily = AF_INET;
    host.Hostname = remoteHost->h_name;
    host.HostIsIP = !strcmp(hostname, host.Hostname);

    return SUCCESS;
}
