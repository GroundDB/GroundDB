#include <memoryPool.hh>
int main()
{
    mempool::connectServer(
        "127.0.0.1",
        122189,
        NULL,
        1);
}