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

    } else if (host.AddressFamily == AF_INET6) {
        sockaddr_in6 *address =
            reinterpret_cast<sockaddr_in6 *>(&host.Address);

        address->sin6_port = htons(host.Port);

    } else {
        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();
        return ERROR_SOCKET_GENERALFAILURE;
    }

    if (net_compat_c::SetNonBlocking(clientSocket) != SUCCESS) {
        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();
        return ERROR_SOCKET_GENERALFAILURE;
    }

    timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    timer_c timer;
    timer.Start();

    result = connect(clientSocket,
                     reinterpret_cast<sockaddr *>(&host.Address),
                     host.AddressLength);

    if (result == 0) {
        time = timer.Stop();

        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return SUCCESS;
    }

#ifdef _WIN32
    int connect_error = WSAGetLastError();

    if (connect_error != WSAEWOULDBLOCK &&
        connect_error != WSAEINPROGRESS &&
        connect_error != WSAEINVAL) {

        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return ERROR_SOCKET_GENERALFAILURE;
    }
#else
    if (errno != EINPROGRESS) {

        int error = errno;

        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return error;
    }
#endif

    fd_set readfds, writefds;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    FD_SET(clientSocket, &readfds);
    FD_SET(clientSocket, &writefds);

    result = select(clientSocket + 1,
                    &readfds,
                    &writefds,
                    NULL,
                    &tv);

    if (result == 0) {
        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return ERROR_SOCKET_TIMEOUT;
    }

    if (result < 0) {
        int error = errno;

        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return error;
    }

    int socket_error = 0;
    socklen_t len = sizeof(socket_error);

    if (getsockopt(clientSocket,
                   SOL_SOCKET,
                   SO_ERROR,
                   reinterpret_cast<char *>(&socket_error),
                   &len) < 0) {

        int error = errno;

        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return error;
    }

    if (socket_error != 0) {

        net_compat_c::Close(clientSocket);
        net_compat_c::Cleanup();

        return socket_error;
    }

    time = timer.Stop();

    net_compat_c::Close(clientSocket);
    net_compat_c::Cleanup();

    return SUCCESS;
}
