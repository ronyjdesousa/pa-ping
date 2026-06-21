# PA-PING

## TCP Ping Utility

Pa-ping is an open-source utility for testing TCP/IP network connectivity
between hosts.

It works similarly to `ping`, but uses TCP connections instead of ICMP echo
requests. This makes it useful for checking whether a remote service is
reachable and accepting connections on a specific port.

Pa-ping can be used to:

- verify whether a remote host is reachable
- test if a TCP port is accepting connections
- measure TCP connection response times
- troubleshoot firewall and network issues
- monitor basic service availability

## Platform Support

Pa-ping currently supports Linux systems.

macOS and Windows support are planned but not yet fully implemented.

## Building from Source

Pa-ping uses the GNU autotools build system.

### Requirements

- autoconf >= 2.71
- automake
- g++
- make

### Build

```sh
./autogen.sh
./configure
make
