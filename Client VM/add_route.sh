#!/bin/bash
sudo route add -net 10.0.8.0/24 tun0
echo "Route added:"
route -n