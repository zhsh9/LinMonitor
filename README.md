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

**Note2:** when using `apt` to install libbpf on your OS like Debian and Ubuntu, there might exist a mismatch between libbpf version and libbpfgo version. You can check the version info by using command `dpkg -l | grep libbpf`, because the `apt` installation resource is not that new enough. In this case, it is advised to build and install `libbpf` from source code.

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

# Grafana Snapshot

![example1](./images/2.png)
