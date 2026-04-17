#!/bin/bash
sudo ifconfig tun0 10.4.2.5/24 up
sudo sysctl net.ipv4.ip_forward=1
sudo route add -host 10.4.2.99 tun0