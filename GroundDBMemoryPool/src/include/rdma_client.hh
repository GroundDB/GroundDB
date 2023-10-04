#pragma once
struct resources *connectServer(
    const char *server_name, /* server host name */
    const int tcp_port,      /* server TCP port */
    const char *ib_devname,  /* server device name. If NULL, client will use the first
                                found device */
    const int ib_port        /* server IB port */
);