#!/bin/bash

## -------------- Install libbpf --------------- ##
sudo apt update
sudo apt install -y gcc build-essential git make libelf-dev strace tar
sudo apt install -y libbpf-dev bpfcc-tools linux-headers-$(uname -r)
sudo apt install -y clang llvm
## -------------- Install libbpf --------------- ##

## -------------- Install ebpf_exporter  ------- ##
# git clone --depth 1 https://github.com/cloudflare/ebpf_exporter.git
## -------------- Install ebpf_exporter  ------- ##

## -------------- Install Prometheus ----------- ##
curl -OL https://github.com/prometheus/prometheus/releases/download/v2.37.8/prometheus-2.37.8.linux-amd64.tar.gz
mv prometheus-2.37.8.linux-amd64.tar.gz ./dependencies
tar -zxvf dependencies/prometheus-2.37.8.linux-amd64.tar.gz -C ./dependencies/
mv ./dependencies/prometheus-2.37.8.linux-amd64 ./dependencies/prometheus
rm ./dependencies/prometheus-2.37.8.linux-amd64
## -------------- Install Prometheus ----------- ##

## -------------- Install Grafana -------------- ##
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
## -------------- Install Grafana -------------- ##