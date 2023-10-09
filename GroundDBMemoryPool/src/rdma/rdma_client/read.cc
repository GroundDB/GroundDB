#include <rdma.hh>
#include "../util.h"

namespace mempool
{

    int rdma_read(
        const struct resources *res, /* RDMA Connection resources */
        const struct memory_region *memreg,
        const struct connection *conn,
        char *buffer,     /* Local buffer to read into. nullptr if it is registered in memreg */
        const size_t lofs,  /* offset in local memory region */
        const size_t size, /* number of bytes to read. -1 if entire menreg is to read */
        const size_t rofs  /* offset in remote memory region */
    )
    {
        if (post_send(res, memreg, conn, IBV_WR_RDMA_READ, lofs, size, rofs))
        {
            fprintf(stderr, "failed to post SR\n");
            return 1;
        }

        if (poll_completion(res, conn))
        {
            fprintf(stderr, "poll completion failed\n");
            return 1;
        }
        if (buffer != nullptr)
            memcpy(buffer, memreg->buf + lofs, size);
        return 0;
    }

} // namespace mempool