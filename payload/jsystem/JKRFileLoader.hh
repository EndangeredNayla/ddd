#pragma once

#include <common/Types.hh>

class JKRFileLoader {
public:
    JKRFileLoader();
    virtual ~JKRFileLoader();
    // ...

protected:
    u8 _04[0x38 - 0x04];
};
size_assert(JKRFileLoader, 0x38);