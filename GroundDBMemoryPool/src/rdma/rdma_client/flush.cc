#include <rdma.hh>
#include "../util.h"

namespace mempool
{

    int rdma_flush(
        const struct resources *res, /* RDMA Connection resources */
        const struct memory_region *memreg,
        const struct connection *conn,
        const char *buffer, /* buffer to flush*/
    )
    {
        if (post_send(res, memreg, conn, IBV_WR_RDMA_WRITE))
        {
            fprintf(stderr, "failed to post SR\n");
            return 1;
        }

        if (poll_completion(res, conn))
        {
            fprintf(stderr, "poll completion failed\n");
            return 1;
        }
        return 0;
    }

} // namespace mempool