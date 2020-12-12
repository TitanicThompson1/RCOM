#!/bin/bash

# Configuring eth0
ifconfig eth0 up
ifconfig eth0 172.16.10.254/24


# Configuring eth1 
fconfig eth1 up
ifconfig eth1 172.16.11.253/24
ifconfig eth1 

# Enabling IP forwarding 
echo 1 > /proc/sys/net/ipv4/ip_forward

# Disabling ICMP echo-ignore-broadcast
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts


# To access the lab network through router
route add -net 172.16.1.0/24 gw 172.16.11.254

# why?
route add default gw 172.16.11.254


# To verify if routes are okay
route -n

# Configuring DNS
echo "services.netlab.fe.up.pt 172.16.1.1" >> /etc/resolv.conf

