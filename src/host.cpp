#include "standard.h"

using namespace std;

pcc_t host_c::IPAddressString()
{
    static char buffer[INET6_ADDRSTRLEN];

    return host_c::GetIPAddressAsString(
        reinterpret_cast<const sockaddr *>(&this->Address),
        buffer,
        sizeof(buffer)
    );
}

int host_c::GetSuccessfulConnectionString(pcc_t str, double time)
{
    int length = 0;
    pcc_t format = i18n_c::GetString(STRING_CONNECT_SUCCESS);

    length = snprintf(NULL, 0, format,
                      this->IPAddressString(),
                      time,
                      socket_c::GetFriendlyTypeName(this->Type),
                      this->Port);

    if (str != NULL) {
        sprintf((pc_t)str, format,
                this->IPAddressString(),
                time,
                socket_c::GetFriendlyTypeName(this->Type),
                this->Port);
    }

    return length;
}

int host_c::GetConnectInfoString(pcc_t str)
{
    int length = 0;
    pcc_t format = NULL;

    if (this->HostIsIP) {
        format = i18n_c::GetString(STRING_CONNECT_INFO_IP);

        length = snprintf(NULL, 0, format,
                          this->Hostname,
                          socket_c::GetFriendlyTypeName(this->Type),
                          this->Port);

        if (str != NULL) {
            sprintf((pc_t)str, format,
                    this->Hostname,
                    socket_c::GetFriendlyTypeName(this->Type),
                    this->Port);
        }
    } else {
        format = i18n_c::GetString(STRING_CONNECT_INFO_FULL);

        length = snprintf(NULL, 0, format,
                          this->Hostname,
                          this->IPAddressString(),
                          socket_c::GetFriendlyTypeName(this->Type),
                          this->Port);

        if (str != NULL) {
            sprintf((pc_t)str, format,
                    this->Hostname,
                    this->IPAddressString(),
                    socket_c::GetFriendlyTypeName(this->Type),
                    this->Port);
        }
    }

    return length;
}

pcc_t host_c::GetIPAddressAsString(const sockaddr *address,
                                   char *buffer,
                                   size_t bufferLength)
{
    const void *ipAddress = NULL;

    if (address->sa_family == AF_INET) {
        const sockaddr_in *ipv4 =
            reinterpret_cast<const sockaddr_in *>(address);

        ipAddress = &ipv4->sin_addr;
    } else if (address->sa_family == AF_INET6) {
        const sockaddr_in6 *ipv6 =
            reinterpret_cast<const sockaddr_in6 *>(address);

        ipAddress = &ipv6->sin6_addr;
    } else {
        snprintf(buffer, bufferLength, "unknown");
        return buffer;
    }

    if (inet_ntop(address->sa_family, ipAddress, buffer, bufferLength) == NULL) {
        snprintf(buffer, bufferLength, "unknown");
    }

    return buffer;
}
