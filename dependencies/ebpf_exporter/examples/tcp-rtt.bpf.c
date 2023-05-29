#include <vmlinux.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "maps.bpf.h"

// Defines the multiplier used to calculate the bucket number
#define BUCKET_MULTIPLIER 10
// Defines the number of buckets in the hash map
#define BUCKET_COUNT 50

// Defines a hash map data structure with a key of u64 and a value of u64
// The map is used to store the round trip time (RTT) of TCP connections
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, BUCKET_COUNT + 2);
    __type(key, u64);
    __type(value, u64);
} tcp_rtt SEC(".maps");

// Function to increment the bucket count and total time of a given RTT
static int do_count(u64 rtt)
{
    // Calculate the bucket number
    u64 bucket = rtt / BUCKET_MULTIPLIER;

    // Increment the bucket count and total time in the hash map
    increment_map(&tcp_rtt, &bucket, 1);
    increment_map(&tcp_rtt, &bucket, rtt);

    return 0;
}

// Kernel probe function called at the entry of the tcp_rcv function
// Records the current time and stores it in the tcp_start hash map using the sock structure as key
SEC("kprobe/tcp_rcv")
int BPF_KPROBE(kprobe__tcp_rcv, struct sock *sk)
{
    u64 start = bpf_ktime_get_ns();
    bpf_map_update_elem(&tcp_start, &sk, &start, BPF_ANY);

    return 0;
}

// Kernel probe function called at the entry of the tcp_ack function
// Calculates the RTT and stores it in the tcp_rtt hash map using the bucket number as key
SEC("kprobe/tcp_ack")
int BPF_KPROBE(kprobe__tcp_ack, struct sock *sk)
{
    u64 start, *pstart;
    u64 rtt, bucket;

    // Lookup the start time of the connection in the tcp_start hash map using the sock structure as key
    pstart = bpf_map_lookup_elem(&tcp_start, &sk);
    if (!pstart)
        return 0;

    start = *pstart;
    // Calculate the RTT
    rtt = (bpf_ktime_get_ns() - start) / 1000;
    // Calculate the bucket number
    bucket = rtt / BUCKET_MULTIPLIER;

    // Increment the bucket count and total time in the hash map
    increment_map(&tcp_rtt, &bucket, 1);
    increment_map(&tcp_rtt, &bucket, rtt);

    // Delete the start time from the tcp_start hash map
    bpf_map_delete_elem(&tcp_start, &sk);

    return 0;
}

// Define the license for the kernel module
char LICENSE[] SEC("license") = "GPL";
