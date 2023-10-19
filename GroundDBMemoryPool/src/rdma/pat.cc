#include <rdma.hh>

namespace mempool{

// Same definition as in OpenAurora/backend/access/logindex/logindex_hashmap.cpp
uint32_t HashKey(KeyType key) {
    uint32_t res = 0;
    res += (key.SpcID&0xFF) * 13 + 7;
    res += (key.DbID&0xFF) * 17 + 5;
    res += (key.RelID&0xFF) * 11 + 7;
    res += (key.ForkNum&0xFF) * 3 + 29;
    if(key.BlkNum != -1) {
        res += (key.BlkNum&0xFF) * 7 + 11;
    }
    return res;
}

uint32_t KeyTypeHashFunction::operator() (const KeyType &key) const {
    return HashKey(key);
}
bool KeyTypeEqualFunction::operator() (const KeyType &key1, const KeyType &key2) const {
    return key1.SpcID == key2.SpcID
        && key1.DbID == key2.DbID
        && key1.RelID == key2.RelID
        && key1.ForkNum == key2.ForkNum
        && key1.BlkNum == key2.BlkNum;
}

} // namespace mempool