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

#ifndef SPAWN_NET_H
#define SPAWN_NET_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPAWN_NET_ENDPOINT_NULL (NULL)
#define SPAWN_NET_CHANNEL_NULL (NULL)

typedef enum spawn_net_type_enum {
  SPAWN_NET_TYPE_NULL = 0, /* netowrk not defined */
  SPAWN_NET_TYPE_TCP  = 1, /* TCP sockets */
  SPAWN_NET_TYPE_FIFO = 2, /* FIFO/pipe */
  SPAWN_NET_TYPE_IBUD = 3, /* IB UD */
} spawn_net_type;

/* represents an endpoint which others may connect to */
typedef struct spawn_net_endpoint_struct {
  int type;         /* network type for endpoint */
  const char* name; /* address of endpoint */
  void* data;       /* network-specific data */
} spawn_net_endpoint;

/* represents an open, reliable channel between two endpoints */
typedef struct spawn_net_channel_struct {
  int type;                 /* network type for channel */
  const char* name;         /* printable name of channel */
  void* data;               /* network-specific data */
} spawn_net_channel;

/* given an endpoint name, identify and return its type */
spawn_net_type spawn_net_infer_type(const char* name);

/* open endpoint for listening */
spawn_net_endpoint* spawn_net_open(spawn_net_type type);

/* close listening endpoint */
int spawn_net_close(spawn_net_endpoint** ep);

/* get name of opened endpoint (pass to others for call to connect) */
const char* spawn_net_name(const spawn_net_endpoint* ep);

/* given an endpoint, get its type */
spawn_net_type spawn_net_get_type(const spawn_net_endpoint* ep);

/* connect to named endpoint (name comes from spawn_net_name) */
spawn_net_channel* spawn_net_connect(const char* name);

/* accept connection on endpoint */
spawn_net_channel* spawn_net_accept(const spawn_net_endpoint* ep);

/* close connection */
int spawn_net_disconnect(spawn_net_channel** ch);

/* read size bytes from connection into buffer */
int spawn_net_read(const spawn_net_channel* ch, void* buf, size_t size);

/* write size bytes from buffer into connection */
int spawn_net_write(const spawn_net_channel* ch, const void* buf, size_t size);

/* wait for data on list of connections, return index of pending comm,
 * if index < neps, it points to an endpoint, otherwise it points to
 * the channel at (index - neps)  */
int spawn_net_wait(
  int neps,                       /* number of endpoints in eps array */
  const spawn_net_endpoint** eps, /* array of endpoints to wait for event */
  int nchs,                       /* number of channels in chs array */
  const spawn_net_channel** chs,  /* array of channels to wait for event */
  int* index                      /* returns index of active item */
);

/* TODO: isend/irecv/waitall */

#ifdef __cplusplus
}
#endif
#endif /* SPAWN_NET_H */
