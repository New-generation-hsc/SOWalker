# SOWalker
an novel second-order graph processing system for random walk

If you want to run in your platform, follow the two steps:
- `preprocess`, the `preprocess` procedure will process the row graph data, and output the binary csr format. The `preprocess` procedure will generate the following files:
    1. `.meta` contains the number of graph vertices and edges
    2. `.vert.blocks` contains the vertex block spliting point
    3. `.edge.blocks` contains the edge block spliting point
    4. `.beg`, `.csr`, `.wht` are the csr binary format, where `.wht` is the edge weight

The `preprocess` commnd
```bash
./bin/test/preprocess /home/hsc/dataset/livejournal/w-soc-livejournal.txt [sorted]

1. ./bin/test/preprocess /home/hsc/dataset/livejournal/w-soc-livejournal.txt
2. ./bin/test/preprocess /home/hsc/dataset/livejournal/w-soc-livejournal.txt sorted
```
- `run`, the `run` procedure will load some blocks into main memory, then perform second-order random walk on them.

The `run` commnd
```bash
./bin/test/node2vec /home/hsc/dataset/livejournal/w-soc-livejournal.txt sample reject length 20 walkpersource 1
```

# Install OR-tools

- ortools : https://developers.google.com/optimization/install/cpp/source_linux

```bash
sudo apt-get -y install git wget pkg-config build-essential cmake autoconf libtool zlib1g-dev lsb-release
git clone https://github.com/google/or-tools
cd ortools && make third_party
make cc
sudo make install_cc
echo "export LD_LIBRARY_PATH=\"$LD_LIBRARY_PATH:/home/hsc/or-tools/lib/\"" >> .bashrc
```