#include <vmlinux.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "maps.bpf.h"

#define BUCKET_MULTIPLIER 10
#define BUCKET_COUNT 50

// Define a hash map data structure with a key of u64 and a value of u64
// The map is used to store the connect time of TCP connections
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, BUCKET_COUNT + 2);
    __type(key, u64);
    __type(value, u64);
} tcp_connect_time SEC(".maps");

// Define a hash map data structure with a key of struct sock pointer and a value of u64
// The map is used to store the start time of TCP connections
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, struct sock *);
    __type(value, u64);
} tcp_connect_start SEC(".maps");

// Function to increment the bucket count and total time of a given connect time
static int do_count(u64 connect_time)
{
    // Calculate the bucket number
    u64 bucket = connect_time / BUCKET_MULTIPLIER;

    // Increment the bucket count and total time in the hash map
    increment_map(&tcp_connect_time, &bucket, 1);
    increment_map(&tcp_connect_time, &bucket, connect_time);

    return 0;
}

// Kprobe function called at the entry of the tcp_v4_conn_request function
// Records the current time and stores it in the tcp_connect_start hash map using the sock structure as key
SEC("kprobe/tcp_v4_conn_request")
int BPF_KPROBE(kprobe__tcp_v4_conn_request, struct sock *sk)
{
    u64 start = bpf_ktime_get_ns();
    bpf_map_update_elem(&tcp_connect_start, &sk, &start, BPF_ANY);

    return 0;
}

// Kprobe function called at the entry of the tcp_v6_conn_request function
// Records the current time and stores it in the tcp_connect_start hash map using the sock structure as key
SEC("kprobe/tcp_v6_conn_request")
int BPF_KPROBE(kprobe__tcp_v6_conn_request, struct sock *sk)
{
    u64 start = bpf_ktime_get_ns();
    bpf_map_update_elem(&tcp_connect_start, &sk, &start, BPF_ANY);

    return 0;
}

// Kprobe function called at the entry of the tcp_v4_conn_established function
// Calculates the connect time and stores it in the tcp_connect_time hash map using the bucket number as key
SEC("kprobe/tcp_v4_conn_established")
int BPF_KPROBE(kprobe__tcp_v4_conn_established, struct sock *sk)
{
    u64 start, *pstart;
    u64 connect_time, bucket;

    // Lookup the start time of the connection in the tcp_connect_start hash map using the sock structure as key
    pstart = bpf_map_lookup_elem(&tcp_connect_start, &sk);
    if (!pstart)
        return 0;

    start = *pstart;
    // Calculate the connect time
    connect_time = (bpf_ktime_get_ns() - start) / 1000;
    // Calculate the bucket number
    bucket = connect_time / BUCKET_MULTIPLIER;

    // Increment the bucket count and total time in the hash map
    increment_map(&tcp_connect_time, &bucket, 1);
    increment_map(&tcp_connect_time, &bucket, connect_time);

    // Delete the start time from the tcp_connect_start hash map
    bpf_map_delete_elem(&tcp_connect_start, &sk);

    return 0;
}

// Kprobe function called at the entry of the tcp_v6_conn_established function
// Calculates the connect time and stores it in the tcp_connect_time hash map using the bucket number as key
SEC("kprobe/tcp_v6_conn_established")
int BPF_KPROBE(kprobe__tcp_v6_conn_established, struct sock *sk)
{
    u64 start, *pstart;
    u64 connect_time, bucket;

    // Lookup the start time of the connection in the tcp_connect_start hash map using the sock structure as key
    pstart = bpf_map_lookup_elem(&tcp_connect_start, &sk);
    if (!pstart)
        return 0;

    start = *pstart;
    // Calculate the connect time
    connect_time = (bpf_ktime_get_ns() - start) / 1000;
    // Calculate the bucket number
    bucket = connect_time / BUCKET_MULTIPLIER;

    // Increment the bucket count and total time in the hash map
    increment_map(&tcp_connect_time, &bucket, 1);
    increment_map(&