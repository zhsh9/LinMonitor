#include <vmlinux.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "maps.bpf.h"

#define BUCKET_MULTIPLIER 10
#define BUCKET_COUNT 50

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, BUCKET_COUNT + 2);
    __type(key, u64);
    __type(value, u64);
} tcp_rtt SEC(".maps");

static int do_count(u64 rtt)
{
    u64 bucket = rtt / BUCKET_MULTIPLIER;

    increment_map(&tcp_rtt, &bucket, 1);
    increment_map(&tcp_rtt, &bucket, rtt);

    return 0;
}

SEC("kprobe/tcp_rcv")
int BPF_KPROBE(kprobe__tcp_rcv, struct sock *sk)
{
    u64 start = bpf_ktime_get_ns();
    bpf_map_update_elem(&tcp_start, &sk, &start, BPF_ANY);

    return 0;
}

SEC("kprobe/tcp_ack")
int BPF_KPROBE(kprobe__tcp_ack, struct sock *sk)
{
    u64 start, *pstart;
    u64 rtt, bucket;

    pstart = bpf_map_lookup_elem(&tcp_start, &sk);
    if (!pstart)
        return 0;

    start = *pstart;
    rtt = (bpf_ktime_get_ns() - start) / 1000;
    bucket = rtt / BUCKET_MULTIPLIER;

    increment_map(&tcp_rtt, &bucket, 1);
    increment_map(&tcp_rtt, &bucket, rtt);

    bpf_map_delete_elem(&tcp_start, &sk);

    return 0;
}

char LICENSE[] SEC("license") = "GPL";
