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

// Now only support single compute node and single simultaneous request
class request_buffer {
public:
    uint8_t *content_;
    size_t size_;
    request_buffer(size_t size): content_(new uint8_t[size]), size_(size) {}
    ~request_buffer() {delete[] content_;}
};

// KeyTypeStruct and HashKey are defined in OpenAurora/include/access/logindex_hashmap.h
struct KeyTypeStruct {
    uint64_t SpcID;
    uint64_t DbID;
    uint64_t RelID;
    uint32_t ForkNum;
    int64_t BlkNum;
};
typedef struct KeyTypeStruct KeyType;
uint32_t HashKey(KeyType key);

class KeyTypeHashFunction{
public:
    uint32_t operator() (const KeyType &key) const;
};
class KeyTypeEqualFunction{
public:
    uint32_t operator() (const KeyType &key1, const KeyType &key2) const;
};

// BLCKSZ is defined in OpenAurora/backend/storage/rpc/rpcclient.cpp
#define BLCKSZ 8192
// Following is only an example. The actual PageGetLSN is defined in "OpenAurora/include/storage/bufpage.h", which needs modification of CMakeLists.txt to include.
#define PageGetLSN(addr) (*(uint64_t*)((char*)(addr) + 20))

int allocate_page(struct memory_region *&memreg, struct resources *res, const char* src, size_t page_cnt, size_t page_size = BLCKSZ);
} // namespace mempool
#include "lru/lru.hh"
#ifdef SERVER
#include <rdma_server.hh>
#else
#include <rdma_client.hh>
#endif