#!/bin/bash
sudo ifconfig tap0 10.0.32.2/24 up
sudo sysctl net.ipv4.ip_forward=1
echo "TAP server configured:"
ifconfig tap0