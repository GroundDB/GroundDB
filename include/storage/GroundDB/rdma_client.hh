#pragma once

namespace mempool
{

    struct resources *connectServer(
        const char *server_name, /* server host name */
        const int tcp_port,      /* server TCP port */
        const char *ib_devname,  /* server device name. If NULL, client will use the first
                                    found device */
        const int ib_port        /* server IB port */
    );

    int rdma_fetch(
        const struct resources *res, /* RDMA Connection resources */
        struct memory_region *memreg,
        const struct connection *conn,
        const size_t lofs = 0,  /* offset in local memory region */
        const size_t size = -1, /* number of bytes to read. -1 if entire menreg is to read */
        const size_t rofs = 0 /* offset in remote memory region */
    );

    int rdma_flush(
        const struct resources *res, /* RDMA Connection resources */
        struct memory_region *memreg,
        const struct connection *conn,
        const size_t lofs = 0,  /* offset in local memory region */
        const size_t size = -1, /* number of bytes to write. -1 denotes entire memreg */
        const size_t rofs = 0 /* offset in remote memory region */
    );

    void init_pat_access_on_client(
        struct resources* res,
        struct request_buffer* pat_req_buf,
        const char *server_name, /* server host name */
        const int tcp_port,     /* server TCP port */
        const int ib_port       /* server IB port */
    );
    // Get the page's remote address and LSN, -1 if not found
    std::pair<uintptr_t, uint64_t> get_page_address_from_mempool(
        const struct resources *res,
        struct memory_region *mr,
        const struct connection *conn,
        KeyType page_id
    );

} // namespace mempool