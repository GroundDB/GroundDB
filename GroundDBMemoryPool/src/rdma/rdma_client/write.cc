#include <rdma.hh>
#include "../util.h"

int rdma_write(
    struct resources *res, /* RDMA Connection resources */
    const char *buffer,    /* Local buffer to write from */
    const size_t size      /* number of bytes to write */
)
{
    char backup[BUFFER_SIZE];
    // Copy buffer to backup. Used for restoring buffer in case of failure
    memcpy(backup, res->buf, BUFFER_SIZE);
    // Attempt to perform rdma write
    memcpy(res->buf, buffer, size);
    if (post_send(res, IBV_WR_RDMA_WRITE))
    {
        fprintf(stderr, "failed to post SR\n");
        /* Restore buffer */
        memcpy(res->buf, backup, BUFFER_SIZE);
        return 1;
    }

    if (poll_completion(res))
    {
        fprintf(stderr, "poll completion failed\n");
        /* Restore buffer */
        memcpy(
            res->buf, backup, BUFFER_SIZE);
        return 1;
    }
    return 0;
}