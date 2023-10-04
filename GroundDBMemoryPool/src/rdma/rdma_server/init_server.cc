#include <rdma.hh>
#include "../util.h"
struct resources *init_server(
    const int tcp_port,     /* server TCP port */
    const char *ib_devname, /* server device name. If NULL, client will use the first
                               found device */
    const int ib_port       /* server IB port */
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
            NULL,
            tcp_port,
            ib_devname,
            ib_port))
    {
        fprintf(stderr, "failed to create resources\n");
        return NULL;
    }
    if (connect_qp(res, NULL, -1, ib_port))
    {
        fprintf(stderr, "failed to connect QPs\n");
        return NULL;
    }
    return res;
}

// int main()
// {
//     struct resources *res = init_server(123443, NULL, 1);
// }