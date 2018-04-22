ifc 8 addr set 10.0.0.111/32
ifc 6 addr set 192.168.1.105/32
xhost +si:localuser:root
ifconfig interface_name IP_address

nc -4uv 10.0.0.105 8808

udp send fe80::40be:73ff:fe5f:e431 8808 hello msg

first we need to assign ip addres to the tap interface
then start the udp server

ifc 8 addr set 10.0.0.111/32
udp server start 8808

udp send4 10.0.0.110 8808 make 8