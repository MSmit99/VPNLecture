#!/bin/bash
echo "=== TAP0 Interface (Layer 2 - notice MAC address) ==="
ifconfig tap0

echo ""
echo "=== ARP across tunnel ==="
sudo arping -I tap0 10.0.32.2

echo ""
echo "=== Ping across tunnel ==="
ping 10.0.32.2