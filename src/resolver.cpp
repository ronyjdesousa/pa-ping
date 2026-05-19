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

    host.IPAddress = *(in_addr*)remoteHost->h_addr_list[0];
    host.Hostname = remoteHost->h_name;
    host.HostIsIP = !strcmp(hostname, host.Hostname);

    return SUCCESS;
}
