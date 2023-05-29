# Table of Contents

- [Table of Contents](#table-of-contents)
- [Features](#features)
- [Requirements](#requirements)
  - [bcc](#bcc)
  - [libbpf](#libbpf)
  - [ebpf\_exporter](#ebpf_exporter)
  - [Prometheus](#prometheus)
  - [Grafana](#grafana)
- [Installation](#installation)
- [Usage](#usage)
- [Showcase](#showcase)
- [Furthermore](#furthermore)
- [eBPF Kernel Side](#ebpf-kernel-side)
- [Kernel Code Explanation](#kernel-code-explanation)
  - [Block IO Latency](#block-io-latency)
  - [TCP SYN Backlog](#tcp-syn-backlog)
  - [TCP Window Clamps](#tcp-window-clamps)
  - [TCP RTT](#tcp-rtt)
  - [Consumption of TCP Connection](#consumption-of-tcp-connection)
- [Time of Network Retransmission](#time-of-network-retransmission)

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
- [x] times of network retransmission

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

- This code is a BPF program designed to trace block I/O operations on Linux. It hooks into three raw tracepoints to capture the start, issue, and completion of block I/O requests. When a request is started or issued, the program records the timestamp. When a request is completed, it calculates the latency by subtracting the start time from the completion time. It then updates a histogram map with the latency value and request metadata, including device, operation, and latency bucket. The histogram is defined in YAML format. The program uses BPF maps to store data and perform lookups. Additionally, it includes macros and helper functions to access and manipulate data structures in a CO-RE compatible way.
- When a block IO request is inserted, the eBPF program is called at the entry points of the `block_rq_insert` and `block_rq_issue` kernel functions to obtain the passed request structure and store the current timestamp. When the block IO request is completed, the program is called at the entry point of the `block_rq_complete` kernel function to obtain the passed request structure and calculate the delay of the block IO request. Then, it divides the delay by a power of 2 to calculate the bucket of the delay and adds the bucket count to a hash table named `bio_latency_seconds`. In addition, the code also includes a metrics YAML configuration, which defines an exponential histogram named `bio_latency_seconds`.
- `biolatency.bpf.c`: Defines three eBPF programs corresponding to block IO request insertion, block IO request issuance, and block IO request completion events. These programs store the latency information of block IO requests in a hash table named bio_latency_seconds.
- `biolatency.yaml`: Defines an exponential histogram named bio_latency_seconds for statistics of block IO request latency information. The latency information is divided into 27 buckets, each bucket size being twice the size of the previous one. The value of each bucket represents the number of block IO requests within the corresponding latency range. In addition, each bucket also records relevant information such as device number, operation type, and latency size.
## TCP SYN Backlog

- Implemented the use of eBPF technology to obtain the size of TCP SYN backlog.
- The eBPF program is called at the entry point of the `tcp_v4_syn_recv_sock` and `tcp_v6_syn_recv_sock` kernel functions to obtain the passed sock structure and read the value of its sk_ack_backlog member. It then divides sk_ack_backlog by 50 to calculate the backlog bucket, and adds the bucket count and backlog count to a hash table named tcp_syn_backlog.
- The code includes a metrics YAML configuration that defines a linear histogram named `tcp_syn_backlog`.

## TCP Window Clamps

- Implemented the use of eBPF technology to obtain the number of times the TCP window is clamped to a low value.
- It is called at the entry and exit points of the kernel function `tcp_try_rmem_schedule`. At the entry point, it inserts the current socket structure pointer into a hash table named tcp_rmem_schedule_enters. At the exit point, it looks up the socket structure pointer from the hash table and calls the handle_tcp_sock function to process the window size of the socket. If the window size is less than a predefined minimum value `MIN_CLAMP`, it increments a counter named tcp_window_clamps_total.
- The code includes a metrics YAML configuration that defines a counter named `tcp_window_clamps_total`.

## TCP RTT

- Implemented the use of eBPF technology to obtain the value of TCP RTT (Round-trip Time).
- It probes at the entry point of the kernel functions `tcp_v4_conn_request` and `tcp_v6_conn_request`, records the current timestamp, and stores the sock structure as the key and the timestamp as the value in a hash table named tcp_start. It then probes again at the entry point of the kernel functions `tcp_v4_conn_established` and `tcp_v6_conn_established`, calculates the rtt value by obtaining the starting timestamp through the sock structure, and stores it in a hash table named tcp_rtt.
- The code includes a metrics YAML configuration that defines a linear histogram named `tcp_rtt`.

## Consumption of TCP Connection

-  This is a BPF program that tracks TCP connection events and logs the duration it takes to set up a connection.
- It does so by utilizing various maps, such as a hash map for tcp_connect_time and tcp_connect_start. The program is equipped with four tracepoints that are established using kprobes. Two of these tracepoints are intended for `tcp_v4_conn_request` and `tcp_v6_conn_request`, both of which record the starting time of the connection by updating the tcp_connect_start map.
- The other two tracepoints, `tcp_v4_conn_established` and `tcp_v6_conn_established`, calculate the connection time and increment the tcp_connect_time map. The tcp_connect_time map is a hash map that utilizes buckets to store connection times. 
- The program further defines a histogram metric for tcp_connect_time, which can be utilized to visualize the distribution of connection times.

# Time of Network Retransmission

- This is a eBPF technology to count the time of network retransmission.
- The eBPF program is called at the entry of the kernel functions `tcp_retransmit_skb` and `tcp_cleanup_rbuf`, obtains the passed sock structure, and records the current timestamp. Then, at the entry of the `tcp_cleanup_rbuf` function, the probe is performed again, the start timestamp is obtained through the sock structure, the network retransmission time is calculated, and it is stored in a hash table named `network_retransmission`.
  - The code defines two hash maps to store data related to network retransmission. The `network_retransmission` map is used to store the retransmission time and frequency, while the `network_retransmission_start` map is used to store the start time of each transmission.
  - The `do_count` function is used to count the retransmission time. It calculates the bucket of the current retransmission time and increments the corresponding entry in the `network_retransmission` map.
  - The `tcp_retransmit_skb` function probe is called at the entry of the `tcp_retransmit_skb` kernel function and records the start time of the current transmission in the `network_retransmission_start` map.
  - The `tcp_cleanup_rbuf` function probe is called at the entry of the `tcp_cleanup_rbuf` kernel function and calculates the retransmission time by subtracting the start time of the transmission from the current time. It then calculates the bucket of the current retransmission time and increments the corresponding entry in the `network_retransmission` map. The entry for the start time of the transmission is then deleted from the `network_retransmission_start` map.
- The code includes a YAML configuration for a linear histogram named `network_retransmission`.
