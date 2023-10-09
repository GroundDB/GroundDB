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

    int rdma_read(
        const struct resources *res, /* RDMA Connection resources */
        const struct memory_region *memreg,
        const struct connection *conn,
        char *buffer,     /* Local buffer to read into. nullptr if it is registered in memreg */
        const size_t lofs = 0,  /* offset in local memory region */
        const size_t size = -1, /* number of bytes to read. -1 if entire menreg is to read */
        const size_t rofs = 0 /* offset in remote memory region */
    );

    int rdma_write(
        const struct resources *res, /* RDMA Connection resources */
        const struct memory_region *memreg,
        const struct connection *conn,
        char *buffer,     /* Local buffer to write from. nullptr if it is registered in memreg */
        const size_t lofs = 0,  /* offset in local memory region */
        const size_t size = -1, /* number of bytes to write. -1 denotes entire memreg */
        const size_t rofs = 0 /* offset in remote memory region */
    );

} // namespace mempool