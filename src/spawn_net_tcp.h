/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by Adam Moody <moody20@llnl.gov>.
 * LLNL-CODE-667277.
 * All rights reserved.
 * This file is part of the SpawnNet library.
 * For details, see https://github.com/hpc/spawnnet
 * Please also read the LICENSE file.
*/

#ifndef SPAWN_NET_TCP_H
#define SPAWN_NET_TCP_H

#include "spawn_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

spawn_net_endpoint* spawn_net_open_tcp();

int spawn_net_close_tcp(spawn_net_endpoint** pep);

spawn_net_channel* spawn_net_connect_tcp(const char* name);

spawn_net_channel* spawn_net_accept_tcp(const spawn_net_endpoint* ep);

int spawn_net_disconnect_tcp(spawn_net_channel** pch);

int spawn_net_read_tcp(const spawn_net_channel* ch, void* buf, size_t size);

int spawn_net_write_tcp(const spawn_net_channel* ch, const void* buf, size_t size);

#ifdef __cplusplus
}
#endif
#endif /* SPAWN_NET_TCP_H */
