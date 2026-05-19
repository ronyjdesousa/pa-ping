#pragma once

#include "standard.h"

class resolver_c {
public:
    static int Resolve(pcc_t hostname, host_c &host);
};
