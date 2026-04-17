#!/bin/bash
sudo ifconfig tap0 10.0.32.1/24 up
echo "TAP client configured:"
ifconfig tap0