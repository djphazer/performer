#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "Routing.h"
#include "FileDefs.h"
#include "core/utils/StringUtils.h"


class BaseTrack {
public:
    static constexpr size_t NameLength = FileHeader::NameLength;

    // trackName
    const char *name() const { return _name; }
    void setName(const char *name) {
        StringUtils::copy(_name, name, sizeof(_name));
    }
    
    protected:
        char _name[NameLength + 1];
};