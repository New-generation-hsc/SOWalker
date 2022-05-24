# SOWalker
an novel second-order graph processing system for random walk

```
./bin/test/node2vec <dataset> [weighted] [sorted] [skip] [blocksize] [nthreads] [dynamic] [sample] [cache_size] [max_iter] [walkpersource] [length] [p] [q]

- dataset:       the dataset path
- weighted:      whether the dataset is weighted
- sorted:        whether the vertex neighbors is sorted
- skip:          whether to skip the interactive preprocess query
- blocksize:     the size of each block
- nthreads:      the number of threads to walk
- dynamic:       whether the blocksize is dynamic, according to the number of walks
- sample:        the sample method, its, alias, reject
- cache_size:    the size(GB) of cache
- max_iter:      the maximum number of iteration for simulated annealing scheduler
- walkpersource: the number of walks for each vertex
- length:        the number of step for each walk
- p:             node2vec parameter
- q:             node2vec parameter
```

## Node2vec

```
./bin/test/node2vec /dataset/livejournal/w-soc-livejournal.txt sample reject length 20 walkpersource 1
```

# Install OR-tools

- ortools : https://developers.google.com/optimization/install/cpp/source_linux

```bash
sudo apt-get -y install git wget pkg-config build-essential cmake autoconf libtool zlib1g-dev lsb-release
git clone https://github.com/google/or-tools
cd or-tools && make third_party
make cc
sudo make install_cc
echo "export LD_LIBRARY_PATH=\"$LD_LIBRARY_PATH:/home/hsc/or-tools/lib/\"" >> ~/.bashrc
source ~/.bashrc
```

# [Option] Install Cmake

```
wget https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1.tar.gz
tar zxvf cmake-3.23.1.tar.gz
cd cmake-3.23.1/
./bootstrap
make      
sudo  make install
```

- [Option] Install Openssl: `sudo apt-get install libssl-dev`