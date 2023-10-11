#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <endian.h>
#include <byteswap.h>
#include <getopt.h>
#include <vector>

#include <sys/time.h>
#include <arpa/inet.h>
#include <infiniband/verbs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace mempool{

/* poll CQ timeout in millisec (2 seconds) */
#define MAX_POLL_CQ_TIMEOUT 2000
#define VERIFIER "VERIFY_CONNECTION"
#define RDMAMSGR "RDMA read operation "
#define RDMAMSGW "RDMA write operation"
#define BUFFER_SIZE 1024 * 1024 /* 1024KB*/
#if __BYTE_ORDER == __LITTLE_ENDIAN
static inline uint64_t htonll(uint64_t x) { return bswap_64(x); }
static inline uint64_t ntohll(uint64_t x) { return bswap_64(x); }
#elif __BYTE_ORDER == __BIG_ENDIAN
static inline uint64_t htonll(uint64_t x) { return x; }
static inline uint64_t ntohll(uint64_t x) { return x; }
#else
#error __BYTE_ORDER is neither __LITTLE_ENDIAN nor __BIG_ENDIAN
#endif

/* structure to exchange data which is needed to connect the QPs */
struct cm_con_data_t
{
    uint64_t addr;   /* Buffer address */
    uint32_t rkey;   /* Remote key */
    uint32_t qp_num; /* QP number */
    uint16_t lid;    /* LID of the IB port */
    uint8_t gid[16]; /* gid */
} __attribute__((packed));

struct connection{
    struct ibv_cq *cq;                 /* CQ handle */
    struct ibv_qp *qp;                 /* QP handle */
    int sock{-1};                      /* TCP socket file descriptor */
};
struct memory_region{
    std::vector<connection> conns;
    struct ibv_mr *mr;                 /* MR handle for buf */
    char *buf;                         /* memory buffer pointer, used for RDMA and send ops */
    bool isBufDeletableFlag;                /* whether buf is exclusive or not */
};
/* structure of system resources */
struct resources
{
    struct ibv_device_attr
        device_attr;
    /* Device attributes */
    struct ibv_port_attr port_attr;    /* IB port attributes */
    struct cm_con_data_t remote_props; /* values to connect to remote side */
    struct ibv_context *ib_ctx;        /* device handle */
    struct ibv_pd *pd;                 /* PD handle */
    std::vector<struct memory_region> memregs;
};

int allocate_page(struct memory_region *&memreg, struct resources *res, const char* src, size_t page_cnt, size_t page_size = 8ull << 10);
} // namespace mempool
#include "lru/lru.hh"
#ifdef SERVER
#include <rdma_server.hh>
#else
#include <rdma_client.hh>
#endif