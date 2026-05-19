#include "standard.h"
#include "net_compat.h"

using namespace std;

int socket_c::Resolve(pcc_t hostname, host_c &host)
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

    clientSocket = socket(AF_INET, socket_c::GetSocketType(host.Type), host.Type);

    if (clientSocket == -1) {
        net_compat_c::Cleanup();
        return ERROR_SOCKET_GENERALFAILURE;
    }

    sockaddr_in clientAddress;

    clientAddress.sin_addr = host.IPAddress;
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(host.Port);

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

    connect(clientSocket, (sockaddr*)&clientAddress, sizeof(clientAddress));

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
