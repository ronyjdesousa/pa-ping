#include "net_compat.h"

int net_compat_c::Init()
{
#ifdef WIN32
    WSADATA wsaData;

    return WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    return SUCCESS;
#endif
}

void net_compat_c::Cleanup()
{
#ifdef WIN32
    WSACleanup();
#endif
}

int net_compat_c::Close(socket_fd_t fd)
{
#ifdef WIN32
    return closesocket(fd);
#else
    return close(fd);
#endif
}

int net_compat_c::SetNonBlocking(socket_fd_t fd)
{
#ifdef WIN32
    ULONG mode = 1;

    return ioctlsocket(fd, FIONBIO, &mode);
#else
    long flags;

    flags = fcntl(fd, F_GETFL, 0);

    if (flags < 0)
        return -1;

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}
