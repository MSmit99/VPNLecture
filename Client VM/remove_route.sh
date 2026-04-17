#!/bin/bash
sudo route del -net 10.0.8.0/24 tun0
echo "Route removed:"
route -n