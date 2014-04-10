/* (C) Copyright 2014, MongoDB, Inc. */

#include "host_types.h"

static const char* hostTypeNames[HOST_TYPE_MAX] = {
    "Standard",
    "Shard Server",
    "Config Server",
    "Mongos"
};

const char* getHostTypeName(int hostType) {
    return hostTypeNames[hostType];
}
