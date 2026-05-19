class host_c
{
public:
    pcc_t Hostname;

    sockaddr_storage Address;
    socklen_t AddressLength;
    int AddressFamily;

    bool HostIsIP;
    int Port;
    int Type;

    pcc_t IPAddressString();

    int GetConnectInfoString(pcc_t str);
    int GetSuccessfulConnectionString(pcc_t str, double time);

    static pcc_t GetIPAddressAsString(const sockaddr *address,
                                      char *buffer,
                                      size_t bufferLength);
};
