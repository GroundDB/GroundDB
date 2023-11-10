#include <unistd.h>
#include "storage/GroundDB/mempool_server.h"
#include "storage/GroundDB/rdma_server.hh"

void
MemPoolMain(int argc, char *argv[],
              const char *dbname,
              const char *username) {
    auto mempool = new mempool::MemPoolManager();
    mempool->init_resources(122189, NULL, 1);
    mempool->init_thread_pool(10);
    mempool->allocate_page_array(1 << 15);
    mempool->init_flush_page_reqbuf();
    mempool->init_access_page_reqbuf();
    mempool->init_sync_pat_reqbuf();
    while(true)
        usleep(1000);
}