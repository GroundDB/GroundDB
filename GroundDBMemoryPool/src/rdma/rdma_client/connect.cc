#include <rdma.hh>
#include "../util.h"
struct resources *connectServer(
    const char *server_name, /* server host name */
    const int tcp_port,      /* server TCP port */
    const char *ib_devname,  /* server device name. If NULL, client will use the first
                                found device */
    const int ib_port        /* server IB port */
)
{
    struct resources *res = (struct resources *)malloc(sizeof(struct resources));
    if (!res)
    {
        fprintf(stderr, "failed to malloc struct resource\n");
        return NULL;
    }

    if (resources_create(
            res,
            server_name,
            tcp_port,
            ib_devname,
            ib_port))
    {
        fprintf(stderr, "failed to create resources\n");
        return NULL;
    }
    if (
        connect_qp(res, server_name, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        return NULL;
    }
    return res;
}

// int main()
// {
//     struct resources *res = connectServer("127.0.0.1", 123443, NULL, 1);
// }