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
        char *buffer,     /* Local buffer to read into */
        const size_t size /* number of bytes to read */
    );

    int rdma_write(
        const struct resources *res, /* RDMA Connection resources */
        const struct memory_region *memreg,
        const struct connection *conn,
        const char *buffer, /* Local buffer to write from */
        const size_t size   /* number of bytes to write */
    );

    int rdma_flush(
        const struct resources *res, /* RDMA Connection resources */
        const struct memory_region *memreg,
        const struct connection *conn,
        const char *buffer /* buffer to flush*/);

} // namespace mempool