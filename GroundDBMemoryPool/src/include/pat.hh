#pragma once

namespace mempool{

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

// Now only support single compute node and single simultaneous request
class request_buffer {
public:
    uint8_t *content_;
    size_t size_;
    request_buffer(size_t size): content_(new uint8_t[size]), size_(size) {}
    ~request_buffer() {delete[] content_;}
};

int allocate_page(struct memory_region *&memreg, struct resources *res, const char* src, size_t page_cnt, size_t page_size = BLCKSZ);

} // namespace mempool