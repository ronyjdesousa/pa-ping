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

int socket_c::Connect(host_c &host, int timeout, double &time)
{
    int result = 0;
    int lastError = ERROR_SOCKET_GENERALFAILURE;

    addrinfo hints;
    addrinfo *addresses = NULL;

    if (net_compat_c::Init() != SUCCESS)
        return ERROR_SOCKET_GENERALFAILURE;

    memset(&hints, 0, sizeof(hints));

    /*
     * Use AF_UNSPEC so the operating system decides the preferred
     * address order, normally following RFC 6724 and /etc/gai.conf.
     */
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = socket_c::GetSocketType(host.Type);
    hints.ai_protocol = host.Type;

    char service[16];
    snprintf(service, sizeof(service), "%d", host.Port);

    result = getaddrinfo(host.Hostname, service, &hints, &addresses);

    if (result != 0 || addresses == NULL) {
        net_compat_c::Cleanup();
        return ERROR_SOCKET_CANNOTRESOLVE;
    }

    for (addrinfo *current = addresses;
         current != NULL;
         current = current->ai_next) {

        socket_fd_t clientSocket;

        clientSocket = socket(current->ai_family,
                              current->ai_socktype,
                              current->ai_protocol);

        if (clientSocket == -1) {
            lastError = ERROR_SOCKET_GENERALFAILURE;
            continue;
        }

        if (net_compat_c::SetNonBlocking(clientSocket) != SUCCESS) {
            net_compat_c::Close(clientSocket);
            lastError = ERROR_SOCKET_GENERALFAILURE;
            continue;
        }

        timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        timer_c timer;
        timer.Start();

        result = connect(clientSocket,
                         current->ai_addr,
                         current->ai_addrlen);

        if (result == 0) {
            time = timer.Stop();

            memset(&host.Address, 0, sizeof(host.Address));
            memcpy(&host.Address, current->ai_addr, current->ai_addrlen);

            host.AddressLength = current->ai_addrlen;
            host.AddressFamily = current->ai_family;

            net_compat_c::Close(clientSocket);
            freeaddrinfo(addresses);
            net_compat_c::Cleanup();

            return SUCCESS;
        }

#ifdef _WIN32
        int connect_error = WSAGetLastError();

        if (connect_error != WSAEWOULDBLOCK &&
            connect_error != WSAEINPROGRESS &&
            connect_error != WSAEINVAL) {

            net_compat_c::Close(clientSocket);
            lastError = ERROR_SOCKET_GENERALFAILURE;
            continue;
        }
#else
        if (errno != EINPROGRESS) {
            lastError = errno;

            net_compat_c::Close(clientSocket);
            continue;
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
            lastError = ERROR_SOCKET_TIMEOUT;
            continue;
        }

        if (result < 0) {
#ifdef _WIN32
            lastError = WSAGetLastError();
#else
            lastError = errno;
#endif
            net_compat_c::Close(clientSocket);
            continue;
        }

        int socket_error = 0;
        socklen_t len = sizeof(socket_error);

        if (getsockopt(clientSocket,
                       SOL_SOCKET,
                       SO_ERROR,
                       reinterpret_cast<char *>(&socket_error),
                       &len) < 0) {
#ifdef _WIN32
            lastError = WSAGetLastError();
#else
            lastError = errno;
#endif
            net_compat_c::Close(clientSocket);
            continue;
        }

        if (socket_error != 0) {
            lastError = socket_error;

            net_compat_c::Close(clientSocket);
            continue;
        }

        time = timer.Stop();

        memset(&host.Address, 0, sizeof(host.Address));
        memcpy(&host.Address, current->ai_addr, current->ai_addrlen);

        host.AddressLength = current->ai_addrlen;
        host.AddressFamily = current->ai_family;

        net_compat_c::Close(clientSocket);
        freeaddrinfo(addresses);
        net_compat_c::Cleanup();

        return SUCCESS;
    }

    freeaddrinfo(addresses);
    net_compat_c::Cleanup();

    return lastError;
}
