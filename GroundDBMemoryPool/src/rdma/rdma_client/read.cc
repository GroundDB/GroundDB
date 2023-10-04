#include <rdma.hh>
#include "../util.h"

int rdma_read(
    const struct resources *res, /* RDMA Connection resources */
    char *buffer,                /* Local buffer to read into */
    const size_t size            /* number of bytes to read */
)
{
    if (post_send(res, IBV_WR_RDMA_READ))
    {
        fprintf(stderr, "failed to post SR\n");
        return 1;
    }

    if (poll_completion(res))
    {
        fprintf(stderr, "poll completion failed\n");
        return 1;
    }
    memcpy(
        buffer,
        res->buf,
        size);
    return 0;
}