#ifndef PARTITION_H
#define PARTITION_H

#include "set.h"

typedef struct partition * PARTITION;

PARTITION partition_init(void **data, size_t sz);

void partition_fini(PARTITION partition);

SET *partition_sets(PARTITION partition);

size_t partition_count_sets(PARTITION partition);

int partition_refine(PARTITION partition, SET set);

void **partition_representatives(PARTITION partition);

SET partition_find_set(PARTITION partition, void *rep);

#endif