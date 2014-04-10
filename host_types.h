/* (C) Copyright 2014, MongoDB, Inc. */

#pragma once

enum HostType {
    HOST_TYPE_STANDARD,
    HOST_TYPE_SHARD,
    HOST_TYPE_CONFIG,
    HOST_TYPE_MONGOS,
    HOST_TYPE_MAX
};

const char* getHostTypeName(int hostType);
