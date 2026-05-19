#pragma once

#include "standard.h"

#ifdef WIN32
typedef SOCKET socket_fd_t;
#else
typedef int socket_fd_t;
#endif

class net_compat_c {
public:
    static int Init();
    static void Cleanup();
    static int Close(socket_fd_t fd);
    static int SetNonBlocking(socket_fd_t fd);
};
