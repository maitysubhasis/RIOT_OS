ifc 8 addr set 10.0.0.105/32
ifc 6 addr set 192.168.1.105/32
xhost +si:localuser:root
ifconfig interface_name IP_address

 nc -4uv 10.0.0.105 8808

first we need to assign ip addres to the tap interface
then start the udp server