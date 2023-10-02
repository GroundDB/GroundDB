#pragma once
#include <stddef.h>

typedef struct remote_memory_pool_client
{
    // TBD
};

struct remote_memory_pool_client connect(
    char *server_ip,
    size_t size,
    int port,
    char *local_buffer,
    size_t local_buffer_size);

void allocate_memory_region(
    size_t size, /* number of bytes to allocate*/
    struct remote_memory_pool_client *rmpc);
void read(size_t mem_reg_id, size_t remote_offset, size_t size, char *local_buffer, struct remote_memory_pool_client *);
void write(size_t mem_reg_id, size_t remote_offset, size_t size, char *local_buffer, struct remote_memory_pool_client *);
#include "add.hh"