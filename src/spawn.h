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

#ifndef SPAWN_H
#define SPAWN_H

/* a single header to include headers from all of spawnnet */

/* memory allocation, error msgs, and packing fns */
#include "spawn_util.h"

/* strmap type and functions */
#include "strmap.h"

/* networking functions: open, connect, read, write */
#include "spawn_net.h"

/* send strmap and strings via spawn_net calls */
#include "spawn_net_util.h"

/* groups and collectives over spawn_net calls */
#include "lwgrp.h"

#endif /* SPAWN_H */
