#pragma once
#include <unordered_map>

namespace mempool{

struct resources* init_server(
    const int tcp_port,     /* server TCP port */
    const char *ib_devname, /* server device name. If NULL, client will use the first
                               found device */
    const int ib_port       /* server IB port */
);

void init_pat_on_server(
    struct resources* res,
    struct page_address_table *pat,
    struct request_buffer* pat_req_buf,
    const int tcp_port,     /* server TCP port */
    const int ib_port       /* server IB port */
);

void get_page_address_service(
    struct resources* res,
    struct memory_region* mr,
    struct connection* conn,
    struct page_address_table *pat
);

} // namespace mempool