#include "standard.h"
#include "net_compat.h"
#include "resolver.h"

using namespace std;

int socket_c::Resolve(pcc_t hostname, host_c &host)
{
    return resolver_c::Resolve(hostname, host);
}

int socket_c::SetPortAndType(int port, int type, host_c &host)
{
    host.Port = port;
    host.Type = type;

    return SUCCESS;
}

pcc_t socket_c::GetFriendlyTypeName(int type)
{
    switch (type) {
    case IPPROTO_TCP:
        return "TCP";

    case IPPROTO_UDP:
        return "UDP";

    default:
        return "UNKNOWN";
    }
}

int socket_c::GetSocketType(int type)
{
    switch (type) {
    case IPPROTO_UDP:
        return SOCK_DGRAM;

    default:
        return SOCK_STREAM;
    }
}

int socket_c::Connect(host_c host, int timeout, double &time)
{
    int result = 0;
    socket_fd_t clientSocket;

    if (net_compat_c::Init() != SUCCESS)
        return ERROR_SOCKET_GENERALFAILURE;

    clientSocket = socket(host.AddressFamily, 
                          socket_c::GetSocketType(host.Type), 
                          host.Type);
    
    if (clientSocket == -1) {
        net_compat_c::Cleanup();
        return ERROR_SOCKET_GENERALFAILURE;
    }

    if (host.AddressFamily == AF_INET) {
        sockaddr_in *address =
        reinterpret_cast<sockaddr_in *>(&host.Address);

       address->sin_port = htons(host.Port);
    }
    
    timeval tv;

    if (net_compat_c::SetNonBlocking(clientSocket) != SUCCESS) {
        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();
        return ERROR_SOCKET_GENERALFAILURE;
    }

    tv.tv_sec = 0;
    tv.tv_usec = timeout * 1000;

    timer_c timer;

    timer.Start();

    connect(clientSocket,
        reinterpret_cast<sockaddr *>(&host.Address),
        host.AddressLength);

    fd_set read, write;

    FD_ZERO(&read);
    FD_ZERO(&write);

    FD_SET(clientSocket, &read);
    FD_SET(clientSocket, &write);

    result = select(clientSocket + 1, &read, &write, NULL, &tv);

    if (result != 1) {
        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return ERROR_SOCKET_TIMEOUT;
    }

    time = timer.Stop();

    if (!FD_ISSET(clientSocket, &read) && !FD_ISSET(clientSocket, &write)) {
        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return ERROR_SOCKET_TIMEOUT;
    }

    net_compat_c::Close(clientSocket);
    net_compat_c::Cleanup();

    return SUCCESS;
}
