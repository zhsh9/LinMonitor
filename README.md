# Features

1. Methodology: combined toolchain
2. Kernel tracing: eBPF
3. Linux based platform: Ubuntu, Debian
4. Data exporter: ebpf_exporter
5. Database: Prometheus
6. Visualization: Grafana

# Requirements

explain the used tools.

Here are the details of dependencies below.

## bcc

**Toolkit and library for efficient BPF-based kernel tracing.**

`BCC` is a toolkit for creating efficient kernel tracing and manipulation programs built upon eBPF, and includes several useful command-line tools and examples. BCC eases writing of eBPF programs for kernel instrumentation in C, includes a wrapper around LLVM, and front-ends in Python and Lua. It also provides a high-level library for direct integration into applications.

The Monitor tool depends on `libbpf` to load eBPF code into the kernel, and you need to have it installed on your system. Note that there's a dependency between `libbpf` version you have installed and `libbpfgo`, which is Go's library to talk to libbpf. Currently we target a stable version, libbpf v1.2.

- Github repo: https://github.com/iovisor/bcc
- Installation: https://github.com/iovisor/bcc/blob/master/INSTALL.md#ubuntu---binary

Installing instructions(Ubuntu-based, by `apt`):

```bash
sudo apt update
sudo apt install bpfcc-tools linux-headers-$(uname -r) -y
```

## libbpf

- Github repo: https://github.com/libbpf/libbpf

The following commands are used to install `libbpf` by `apt` on Debian or Ubuntu.

```bash
sudo apt update
sudo apt install -y gcc build-essential git make libelf-dev strace tar bpfcc-tools libbpf-dev linux-headers-$(uname -r)
sudo apt install -y clang llvm
```

## ebpf_exporter

- Github repo: https://github.com/cloudflare/ebpf_exporter

`ebpf_exporter` depends on `libbpf` to load eBPF code into the kernel, and you need to have it installed on your system. Alternatively, you can use the bundled `Dockerfile` to have libbpf compiled in there.

**Note1:** there's a dependency between `libbpf` version you have installed and `libbpfgo`, which is Go's library to talk to libbpf. Currently we target `libbpf v1.2`, which has a stable interface.

**Note2:** when using `apt` to install libbpf on your OS like Debian and Ubuntu, there might exist a mismatch between libbpf version and libbpfgo version. You can check the version info by using command `dpkg -l | grep libbpf`, because the software repository of `apt` on Debian and Ubuntu (2018LTS, 2020LTS, 2022LTS and so on) is not that new enough. In this case, it is advised to build and install `libbpf` from source code.

We compile ebpf_exporter with libbpf `statically` compiled in, so there's only ever a chance of build time issues, never at run time.

There are two ways to build the binary file of ebpf_exporter (if you encounter really hard problems of building on the host, building in Docker is a good alternative):

```bash
# Build on host.
make clean build
make -C examples clean build

# Build on Docker.
docker build -t ebpf_exporter .
docker cp $(docker create ebpf_exporter):/ebpf_exporter ./
make -C examples clean build
```

## Prometheus

Prometheus is a free software application used for event monitoring and alerting. It records metrics in a time series database built using an HTTP pull model, with flexible queries and real-time alerting. The basic structure of Prometheus is shown as below.

![Prometheus](./images/1.png)

- Installation: https://prometheus.io/download/, Prometheus v2.37.8
- Document: https://prometheus.io/docs/introduction/overview/

Installing instructions(Ubuntu-based):

```bash
curl -OL https://github.com/prometheus/prometheus/releases/download/v2.37.8/prometheus-2.37.8.linux-amd64.tar.gz
tar -zxvf prometheus-2.37.8.linux-amd64.tar.gz
```

## Grafana

- Installation: https://grafana.com/docs/grafana/latest/setup-grafana/installation/debian/
- Document: https://grafana.com/docs/grafana/latest/

Grafana Enterprise vs OSS: Grafana Enterprise is the recommended and default edition. It is available for free and includes all the features of the OSS edition. You can also upgrade to the full Enterprise feature set, which has support for Enterprise plugins.

Installing instructions(Ubuntu-based, by `apt`):

```bash
# To install required packages and download the Grafana repository signing key, run the following commands:
sudo apt install -y apt-transport-https
sudo apt install -y software-properties-common wget
sudo wget -q -O /usr/share/keyrings/grafana.key https://apt.grafana.com/gpg.key

# To add a repository for stable releases, run the following command:
echo "deb [signed-by=/usr/share/keyrings/grafana.key] https://apt.grafana.com stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list

# To add a repository for beta releases, run the following command:
echo "deb [signed-by=/usr/share/keyrings/grafana.key] https://apt.grafana.com beta main" | sudo tee -a /etc/apt/sources.list.d/grafana.list

# After you add the repository, run the following commands to install the OSS or Enterprise release:
# Update the list of available packages
sudo apt update
# Install the latest OSS release:
sudo apt install -y grafana
# Install the latest Enterprise release:
sudo apt install -y grafana-enterprise
```

# Installation

explain how to install this tool.

```bash
git clone https://github.com/zhsh9/LinMonitor.git
cd LinMonitor
# use Makefile to install dependencies.
make prepare

# or use script to install dependencies.
chmod +x ./install_dependencies.sh
./install_dependencies.sh
```

You should change details inside `install_dependencies.sh` if dipendencies directory is empty.

# Usage

explain how to use this tool.

```bash
make build  # build binary files of dependencies.
make server # start servers.
make clean  # kill processes of servers.
```

Enter `http://localhost:3000` in a browser to configure Grafana panels.

# Showcase

Grafana Snapshot: 

![example1](./images/2.png)

Current complementation of indices from Linux kernel:

- [x] `bio_latency_seconds`: TCP SYN backlog size
- [x] `tcp_syn_backlog`: TCP SYN backlog size
- [x] `tcp_window_clamps_total`: Number of times that TCP window was clamped to a low value

# Furthermore

Extract more indices from Linux kernel:  
(thourgh more file inside `./dependencies/ebpf_exporter/examples/xxx.bpf.c and xxx.yaml`)

TODO:

- [X] time consumption of tcp connection
  - `tcp_connect_time`: Time Consumption of TCP Connection
- [x] rtt(tound-trip time) of tcp
  - `tcp_rtt`: TCP round trip time
- [ ] times of network retransmission

# eBPF Kernel Side

- tcp rtt(round-trip time):
  - `tcp-rtt.bpf.c` and `tcp-rtt.yaml`
- time consumption of tcp connection:
  - `tcp-connection-consumption.bpf.c` and `tcp-connection-consumption.bpf.yaml`
- time of network retransmission:
  - `network-retransmission-time.bpf.c` and `network-retransmission-time.bpf.yaml`

# Kernel Code Explanation

- `bits.bpf.h`
  - This defines two inline functions, log2 and log2l, both of which compute the base-2 logarithm of their input values. In both functions, the logarithm is computed using a bit-twiddling algorithm that counts the number of leading zero bits in the input value and subtracts the count from the bit width of the input value minus one.
  - The log2 function takes an unsigned 32-bit integer input and returns the integer log2 value as an unsigned 64-bit integer.
  - The log2l function takes an unsigned 64-bit integer input and returns the integer log2 value as an unsigned 64-bit integer.
- `maps.bpf.h`
  - This defines a function called increment_map, which takes as input a BPF map, a key, and an increment value. The function looks up the value associated with the key in the map. If no value is found, it adds a new key-value pair with an initial value of zero. The function then atomically increments the value associated with the key by the given increment value and returns the new value.
- `regs-ip.bpf.h`
  - defines a macro KPROBE_REGS_IP_FIX, which performs different operations based on the architecture. For x86 architecture, the macro subtracts sizeof(kprobe_opcode_t) from IP. For other architectures, the macro does nothing.
  - This is used for kprobes and is used to handle the IP address during function call in order to obtain the function address in a BPF program. For the x86 architecture, entry is the function IP + sizeof(kprobe_opcode_t), so in the context of a BPF program, it is necessary to subtract sizeof(kprobe_opcode_t) in order to obtain the function address.

## Block IO Latency

- 使用eBPF技术实现了块IO（Block IO）延迟的计数。当块IO请求被插入时，eBPF程序会在`block_rq_insert`和`block_rq_issue`内核函数入口处被调用，获取所传递的request结构体，并存储当前时间戳。当块IO请求完成时，程序会在`block_rq_complete`内核函数入口处被调用，获取所传递的request结构体，并计算出块IO请求的延迟。然后，它将延迟除以2的幂，计算出延迟的bucket，分别将bucket计数加入名为`bio_latency_seconds`的哈希表中。此外，代码中还包括了一个metrics的YAML配置，定义了一个名为bio_latency_seconds的指数直方图(Histogram)。
- `biolatency.bpf.c`中定义了三个eBPF程序，分别对应于块IO请求插入、块IO请求发出和块IO请求完成三个事件。这些程序在处理事件时，会将块IO请求的延迟信息存储到名为bio_latency_seconds的哈希表中。
- `biolatency.yaml`中定义了一个名为bio_latency_seconds的指数直方图(Histogram)，用于对块IO请求的延迟信息进行统计。其中，延迟信息被分为了27个bucket，每个bucket的大小是前一个bucket大小的两倍。每个bucket的值表示对应延迟范围内的块IO请求数量。另外，每个bucket还会记录设备号、操作类型和延迟大小等相关信息。

This code is a BPF program designed to trace block I/O operations on Linux. It hooks into three raw tracepoints to capture the start, issue, and completion of block I/O requests. When a request is started or issued, the program records the timestamp. When a request is completed, it calculates the latency by subtracting the start time from the completion time. It then updates a histogram map with the latency value and request metadata, including device, operation, and latency bucket. The histogram is defined in YAML format at the end of the code. The program uses BPF maps to store data and perform lookups. Additionally, it includes macros and helper functions to access and manipulate data structures in a CO-RE compatible way.

## TCP SYN Backlog

- 使用eBPF技术实现了获取TCP SYN backlog的大小。
- eBPF程序会在`tcp_v4_syn_recv_sock`和`tcp_v6_syn_recv_sock`内核函数入口处被调用，获取所传递的sock结构体，并读取其sk_ack_backlog成员的值。然后，它将sk_ack_backlog除以50，计算出backlog的bucket，分别将bucket计数和backlog计数加入名为tcp_syn_backlog的哈希表中。
- 代码中包括了一个metrics的YAML配置，定义了一个名为tcp_syn_backlog的线性直方图(Histogram)。

## TCP Window Clamps

- 使用eBPF技术实现了获取TCP窗口被钳制为低值的次数。
- 在内核函数`tcp_try_rmem_schedule`的入口处和出口处被调用。在入口处，它会将当前socket结构体指针插入一个名为tcp_rmem_schedule_enters的哈希表中。在出口处，它会从哈希表中查找socket结构体指针，然后调用handle_tcp_sock函数处理该socket的窗口大小。如果窗口大小小于一个预定义的最小值`MIN_CLAMP`，它将会递增名为tcp_window_clamps_total的计数器。
- 代码中包括一个metrics的YAML配置，用于定义一个名为tcp_window_clamps_total的计数器。

## TCP RTT

- 使用eBPF技术实现了获取TCP RTT(Round-trip Time)的值。
- 在`tcp_v4_conn_request`和`tcp_v6_conn_request`内核函数入口处进行探针，记录当前时间戳，并将sock结构体作为key，时间戳作为value，存储到名为tcp_start的哈希表中。在tcp_v4_conn_established和tcp_v6_conn_established内核函数入口处再次进行探针，通过sock结构体获取起始时间戳，并计算出rtt值，将其存储到名为tcp_rtt的哈希表中。
- 代码中包括了一个metrics的YAML配置，定义了一个名为tcp_rtt的线性直方图(Histogram)。

## Consumption of TCP Connection

-  The code is a BPF program that tracks TCP connection events and logs the duration it takes to set up a connection.
- It does so by utilizing various maps, such as a hash map for tcp_connect_time and tcp_connect_start. The program is equipped with four tracepoints that are established using kprobes. Two of these tracepoints are intended for `tcp_v4_conn_request` and `tcp_v6_conn_request`, both of which record the starting time of the connection by updating the tcp_connect_start map.
- The other two tracepoints, `tcp_v4_conn_established` and `tcp_v6_conn_established`, calculate the connection time and increment the tcp_connect_time map. The tcp_connect_time map is a hash map that utilizes buckets to store connection times. 
- The program further defines a histogram metric for tcp_connect_time, which can be utilized to visualize the distribution of connection times.
