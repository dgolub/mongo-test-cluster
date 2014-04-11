/* (C) Copyright 2014, MongoDB, Inc. */

#include "host_types.h"

#include <string.h>

static const char* hostTypeNames[HOST_TYPE_MAX] = {
    "Standard",
    "Shard Server",
    "Config Server",
    "Mongos"
};

static const char* hostTypeInternalNames[HOST_TYPE_MAX] = {
    "STANDARD",
    "SHARD",
    "CONFIG",
    "MONGOS"
};

const char* getHostTypeName(int hostType) {
    return hostTypeNames[hostType];
}

const char* getHostTypeInternalName(int hostType) {
    return hostTypeInternalNames[hostType];
}

int findHostTypeByInternalName(const char* internalName) {
    for (int i = 0; i < HOST_TYPE_MAX; i++) {
        if (strcmp(internalName, hostTypeInternalNames[i]) == 0) {
            return i;
        }
    }
    return -1;
}
