package main

//go:generate go run github.com/cilium/ebpf/cmd/bpf2go atomic atomic.c

import (
	"flag"
	"log"
	"net"
	"time"

	"github.com/cilium/ebpf"
	"github.com/cilium/ebpf/link"
	"github.com/cilium/ebpf/rlimit"
)

func main() {
	// Allow the current process to lock memory for eBPF resources.
	if err := rlimit.RemoveMemlock(); err != nil {
		log.Fatal(err)
	}

	var ifname string
	flag.StringVar(&ifname, "i", "lo", "Network interface name where the eBPF program will be attached")
	flag.Parse()

	// Load pre-compiled programs and maps into the kernel.
	var atomicObjs atomicObjects
	atomicObjs = atomicObjects{}
	if err := loadAtomicObjects(&atomicObjs, nil); err != nil {
		log.Fatal(err)
	}

	iface, err := net.InterfaceByName(ifname)
	if err != nil {
		log.Fatalf("Getting interface %s: %s", ifname, err)
	}

	var key uint64 = 0
	var value uint64 = 0
	err = atomicObjs.atomicMaps.HashMap.Update(&key, &value, ebpf.UpdateAny)
	if err != nil {
		log.Fatalf("Failed to update the map: %v", err)
	}

	// Attach XDP program to the network interface.
	xdplink, err := link.AttachXDP(link.XDPOptions{
		Program:   atomicObjs.AtomicOps,
		Interface: iface.Index,
	})
	if err != nil {
		log.Fatal("Attaching XDP:", err)
	}
	defer xdplink.Close()

	time.Sleep(time.Second * 10)
}
