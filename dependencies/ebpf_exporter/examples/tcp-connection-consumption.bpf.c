#include <vmlinux.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "maps.bpf.h"

#define BUCKET_MULTIPLIER 10
#define BUCKET_COUNT 50

// Define the tcp_connect_time map
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, BUCKET_COUNT + 2);
    __type(key, u64);
    __type(value, u64);
} tcp_connect_time SEC(".maps");

// Define the tcp_connect_start map
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, struct sock *);
    __type(value, u64);
} tcp_connect_start SEC(".maps");

// Function to increment the tcp_connect_time map
static int do_count(u64 connect_time)
{
    u64 bucket = connect_time / BUCKET_MULTIPLIER;

    increment_map(&tcp_connect_time, &bucket, 1);
    increment_map(&tcp_connect_time, &bucket, connect_time);

    return 0;
}

// Tracepoint for tcp_v4_conn_request
SEC("kprobe/tcp_v4_conn_request")
int BPF_KPROBE(kprobe__tcp_v4_conn_request, struct sock *sk)
{
    // Record the start time of the connection
    u64 start = bpf_ktime_get_ns();
    bpf_map_update_elem(&tcp_connect_start, &sk, &start, BPF_ANY);

    return 0;
}

// Tracepoint for tcp_v6_conn_request
SEC("kprobe/tcp_v6_conn_request")
int BPF_KPROBE(kprobe__tcp_v6_conn_request, struct sock *sk)
{
    // Record the start time of the connection
    u64 start = bpf_ktime_get_ns();
    bpf_map_update_elem(&tcp_connect_start, &sk, &start, BPF_ANY);

    return 0;
}

// Tracepoint for tcp_v4_conn_established
SEC("kprobe/tcp_v4_conn_established")
int BPF_KPROBE(kprobe__tcp_v4_conn_established, struct sock *sk)
{
    u64 start, *pstart;
    u64 connect_time, bucket;

    // Get the start time of the connection from tcp_connect_start map
    pstart = bpf_map_lookup_elem(&tcp_connect_start, &sk);
    if (!pstart)
        return 0;

    start = *pstart;

    // Calculate the connection time and the bucket
    connect_time = (bpf_ktime_get_ns() - start) / 1000;
    bucket = connect_time / BUCKET_MULTIPLIER;

    // Increment the tcp_connect_time map
    increment_map(&tcp_connect_time, &bucket, 1);
    increment_map(&tcp_connect_time, &bucket, connect_time);

    // Delete the entry from tcp_connect_start map
    bpf_map_delete_elem(&tcp_connect_start, &sk);

    return 0;
}

// Tracepoint for tcp_v6_conn_established
SEC("kprobe/tcp_v6_conn_established")
int BPF_KPROBE(kprobe__tcp_v6_conn_established, struct sock *sk)
{
    u64 start, *pstart;
    u64 connect_time, bucket;

    // Get the start time of the connection from tcp_connect_start map
    pstart = bpf_map_lookup_elem(&tcp_connect_start, &sk);
    if (!pstart)
        return 0;

    start = *pstart;

    // Calculate the connection time and the bucket
    connect_time = (bpf_ktime_get_ns() - start) / 1000;
    bucket = connect_time / BUCKET_MULTIPLIER;

    // Increment the tcp_connect_time map
    increment_map(&tcp_connect_time, &bucket, 1);
    increment_map(&tcp_connect_time, &bucket, connect_time);

    // Delete the entry from tcp_connect_start map
    bpf_map_delete_elem(&tcp_connect_start, &sk);

    return 0;
}

// Define the license
char LICENSE[] SEC("license") = "GPL";
