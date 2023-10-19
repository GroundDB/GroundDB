#pragma once
#include "c.h"
#include "access/logindex_hashmap.h"
#include "storage/bufpage.h"

// todo: OpenAurora/include/port.h define following macros which causes link error. We undef them for now.
#undef printf
#undef sprintf
#undef fprintf

namespace mempool{

class KeyTypeHashFunction{
public:
    uint32_t operator() (const KeyType &key) const;
};
class KeyTypeEqualFunction{
public:
    bool operator() (const KeyType &key1, const KeyType &key2) const;
};

/* The current design deploy unordered_map which is not memory continuous and thus
inaccessible by one-sided RDMA. Further discussion and potential re-design are necessary. */
class page_address_table {
public:
    std::unordered_map<KeyType, uintptr_t, KeyTypeHashFunction, KeyTypeEqualFunction> pat;
    page_address_table(){}
};

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