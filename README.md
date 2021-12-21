### IndexedPriorityQueue：可索引的优先级队列

IndexedPriorityQueue是C++编程语言实现的开源的可以快速改变某个指定元素值的优先级队列。它通过为优先级队列中每个元素维护一个索引Index值来实现快速修改特定元素值且又保证堆性质的目的。修改元素值得操作为IndexedPriorityQueue::Update方法，它执行的时间复杂度为O（logN) (N为优先队列的大小)。这种数据结构的一个典型应用就是求取海量数据中按某个维度去重后的按另一个维度排序的TopN元素集合。本项目的单元测试代码中给出了该应用场景的一个典型求解方案。

本项目还具有其它如下优势：

1. 本项目中整个数据结构的定义与实现都包含在一个头文件中，且其核心实现不依赖任何第三方库，独立而又精炼，可以直接拷贝该头文件到您的项目直接include，快速满足您的数据结构功能要求。 （注意运行本项目的单元测试代码仍依赖于第三方库cppunit和tulip-log模块，项目中已经包含其源码和编译方法。）
2. 本项目实现的IndexedPriorityQueue数据结构的正确性和性能已经被单元测试中大量实例验证和确认，可放心使用。单元测试部分代码同时还可为您提供关于IndexedPriorityQueue的详实的用法指引。



### 编译和安装

目前支持类Unix环境下编译安装，IndexedPriorityQueue 项目依赖的第三方库有：
- tulip-log
- cppunit 

编译安装IndexedPriorityQueue之前先要安装以上2个依赖库。

具体编译和安装方法如下：

```
git clone https://github.com/apollo008/indexed-priority-queue.git indexed-priority-queue.git
cd indexed-priority-queue.git
vim src/CMakeLists.txt 
搜索need to set 发现存在第3行和第11行，依次修改如下：
第3行： set(TOP_PROJECT_NAME IndexedPriorityQueue ) 将IndexedPriorityQueue改为你的实际项目名称
第11行： set(${TOP_PROJECT_NAME_UPPER}_DEPEND_PREFIX_DIR /path/to/install/share) 将/path/to/install/share改为share库的安装路径，比如${HOME}/local,注意确保该路径有写权限。修改之后后续不要再改变该路径。

mkdir build-dir
cd build-dir
#安装依赖
cmake -DENABLE_BUILD_SHARE=ON ../src
执行完以上这一步即可完成依赖库。注意不需要再执行make 和make install了。

接下来编译安装indexed-priority-queue,包含单元测试。
#安装indexed-priority-queue
首先保持根目录下CMakeLists.txt第11行修改的内容不再改变；
cd build-dir
rm -rf *
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install  ../src
make -j10
#此时 运行ctest 命令可以执行所有的单元测试
make install

```

执行完以上，即可在/path/to/install目录下生成bin、lib、include3个目录。

注意：运行程序bin/indexedpq_test 时，需要在当前目录放置tulip-log的日志配置文件logger.conf ，同时该目录下要已经创建好logs 目录供输出文件名滚动的日志文件。logger.conf文件示例见misc/config/logger.conf 。

在build-dir目录下执行ctest，运行结果如下：

![image-20211221182405979](https://raw.githubusercontent.com/apollo008/picgo/master/img/20211221182406.png)

查看本地的Testing/Temporary/LastTest.log可看到具体的fail细节。

### 其它

相关细节或其它未尽事宜，可联系 dingbinthu@163.com 探讨咨询。

