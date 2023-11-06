# Memory-Pool

## Deployment

1. Download postgresql-13.0.tar.gz and uncompress.

   ```
   tar -zxvf postgresql-13.0.tar.gz
   ```

2. Initialize GroundDB git repository in src directroy.

   ```
   cd postgresql-13.0/src
   git remote add origin git@github.com:GroundDB/GroundDB.git
   git fetch
   git reset --hard origin/$TAG
   git checkout $TAG
   ```

3. Configure, make, and install.

   ```
   cd ..
   ./configure --prefix=$INSTALL_DIR LDFLAGS='-std=c++17 -lstdc++ -lrocksdb -lthrift -lrt -ldl -lsnappy -lgflags -lz -lbz2 -llz4 -lzstd -lrocksdb -lpthread -libverbs -I. -I/usr/local/include -I/usr/include -L/usr/local/lib -L/usr/bin'
   make
   make install
   ```

