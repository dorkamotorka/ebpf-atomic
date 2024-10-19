//go:build ignore
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 1);
  __type(key, __u64);
  __type(value, __u64);
} hash_map SEC(".maps");

SEC("xdp")
int atomic_ops(struct xdp_md *ctx) {
  __u64 key = 0;
  __u64 *value;

  // Fetch value from the map
  value = bpf_map_lookup_elem(&hash_map, &key);
  if (!value) {
    return XDP_ABORTED;
  }

  // Read value, add 5 and return the new value
  int new_val_add = __sync_fetch_and_add(value, 5);
  bpf_printk("After __sync_fetch_and_add: %d", new_val_add);

  // Read value, subtract 2 and return the new value
  int new_val_sub = __sync_fetch_and_sub(value, 2);
  bpf_printk("After __sync_fetch_and_sub: %d", new_val_sub);

  // Read value, binary OR with 1 and return the new value
  int new_val_or = __sync_fetch_and_or(value, 1);
  bpf_printk("After __sync_fetch_and_or: %d", new_val_or);

  // Read value, binary XOR with 3 and return the new value
  int new_val_xor = __sync_fetch_and_xor(value, 3);
  bpf_printk("After __sync_fetch_and_xor: %d", new_val_xor);

  // Read value, check if it is equal to expected_val. If true return the
  // original value. Otherwise return 100.
  int expected_val = *value;
  int new_val_cas = __sync_val_compare_and_swap(value, expected_val, 100);
  bpf_printk("After __sync_val_compare_and_swap: %d", new_val_cas);

  // Read value, write 200 to it, return original value
  int new_val_set = __sync_lock_test_and_set(value, 200);
  bpf_printk("After __sync_lock_test_and_set: %d", new_val_set);

  return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
