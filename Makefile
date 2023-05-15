# Monitored Linux url.
url = localhost
# Ports that servers use.
ebpf_port       = 9435
prometheus_port = 9090
grafana_port    = 3000
# Binary file that servers use.
ebpf       = ./dependencies/ebpf_exporter/ebpf_exporter
prometheus = ./dependencies/prometheus/prometheus
grafana    = /usr/sbin/grafana
# Configuration files that ebpf uses.
examples = ./dependencies/ebpf_exporter/examples
example  = ./dependencies/ebpf_exporter/examples/biolatency

prepare: install_dependencies.sh
	chmod +x ./install_dependencies.sh
	./install_dependencies.sh

build:
	cd ./dependencies/ebpf_exporter
	make build
	make -C examples clean build
	cd ../..
#   TODO: write more ebpf c files to catch different data from linux kernel.

server:
	sudo ebpf --config.dir $(examples) --config.names $(example)
	prometheus --config.file=./prometheus.yaml
	sudo grafana server

clean:
	sudo lsof -i:$(ebpf_port) | grep $(ebpf_port) | awk '{print $2}' | uniq | xargs sudo kill -9
	sudo lsof -i:$(prometheus_port) | grep $(prometheus_port) | awk '{print $2}' | uniq | xargs sudo kill -9
	sudo lsof -i:$(grafana_port) | grep $(grafana_port) | awk '{print $2}' | uniq | xargs sudo kill -9
