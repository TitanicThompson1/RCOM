#!/bin/bash

# Configuring tux3
ifconfig eth0 up
ifconfig eth0 172.16.10.1/24

# Adding a route for tux3 to know how to contact with tux2
route add -net 172.16.11.0/24 gw 172.16.10.254

# why?
route add default gw 172.16.11.254

# To verify if routes are okay
route -n

# Configuring DNS
echo "services.netlab.fe.up.pt 172.16.1.1" >> /etc/resolv.conf