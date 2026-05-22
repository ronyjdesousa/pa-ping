#pragma once

#include "standard.h"

class resolver_c {
public:
    static int Resolve(pcc_t hostname,
                       int addressFamily,
                       host_c &host);
};
