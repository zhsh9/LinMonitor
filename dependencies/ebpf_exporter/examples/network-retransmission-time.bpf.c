#include <vmlinux.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "maps.bpf.h"

#define BUCKET_MULTIPLIER 10
#define BUCKET_COUNT 50

// define a hash map to store retransmission time and frequency
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, BUCKET_COUNT + 2);
    __type(key, u64);
    __type(value, u64);
} network_retransmission SEC(".maps");

// define a hash map to store start time of each transmission
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, struct sock *);
    __type(value, u64);
} network_retransmission_start SEC(".maps");

// function to count the retransmission time
static int do_count(u64 retransmission_time)
{
    u64 bucket = retransmission_time / BUCKET_MULTIPLIER;

    increment_map(&network_retransmission, &bucket, 1);
    increment_map(&network_retransmission, &bucket, retransmission_time);

    return 0;
}

// probe for tcp_retransmit_skb function
SEC("kprobe/tcp_retransmit_skb")
int BPF_KPROBE(kprobe__tcp_retransmit_skb, struct sock *sk)
{
    u64 start = bpf_ktime_get_ns();
    bpf_map_update_elem(&network_retransmission_start, &sk, &start, BPF_ANY);

    return 0;
}

// probe for tcp_cleanup_rbuf function
SEC("kprobe/tcp_cleanup_rbuf")
int BPF_KPROBE(kprobe__tcp_cleanup_rbuf, struct sock *sk)
{
    u64 start, *pstart;
    u64 retransmission_time, bucket;

    pstart = bpf_map_lookup_elem(&network_retransmission_start, &sk);
    if (!pstart)
        return 0;

    start = *pstart;
    retransmission_time = (bpf_ktime_get_ns() - start) / 1000;
    bucket = retransmission_time / BUCKET_MULTIPLIER;

    increment_map(&network_retransmission, &bucket, 1);
    increment_map(&network_retransmission, &bucket, retransmission_time);

    bpf_map_delete_elem(&network_retransmission_start, &sk);

    return 0;
}

// GPL license
char LICENSE[] SEC("license") = "GPL";
