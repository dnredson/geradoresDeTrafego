#!/bin/bash
docker run -itd --name h1  --privileged -v shared:/codes --workdir /codes dnredson/hostup
docker run -itd --name h2   --privileged -v shared:/codes  --workdir /codes dnredson/hostup
docker run -itd --name sw1  --network="host" --privileged -v shared:/codes  --workdir /codes dnredson/p4d
docker run -itd --name sw2  --network="host" --privileged -v shared:/codes  --workdir /codes dnredson/p4d
docker run -itd --name sw3  --network="host" --privileged -v shared:/codes  --workdir /codes dnredson/p4d
docker run -itd --name sw4  --network="host" --privileged -v shared:/codes  --workdir /codes dnredson/p4d

sudo ip link add veth1 type veth peer name veth2
sudo ip link add veth3 type veth peer name veth4
sudo ip link add veth5 type veth peer name veth6
sudo ip link add veth7 type veth peer name veth8
sudo ip link add veth9 type veth peer name veth10

PID1=$(docker inspect -f '{{.State.Pid}}' h1)
PID2=$(docker inspect -f '{{.State.Pid}}' sw1)
PID3=$(docker inspect -f '{{.State.Pid}}' sw2)
PID4=$(docker inspect -f '{{.State.Pid}}' sw3)
PID5=$(docker inspect -f '{{.State.Pid}}' sw4)
PID6=$(docker inspect -f '{{.State.Pid}}' h2)

sudo ip link set veth1 netns $PID1
sudo ip link set veth2 netns $PID2
sudo ip link set veth3 netns $PID2
sudo ip link set veth4 netns $PID3
sudo ip link set veth5 netns $PID3
sudo ip link set veth6 netns $PID4
sudo ip link set veth7 netns $PID4
sudo ip link set veth8 netns $PID5
sudo ip link set veth9 netns $PID5
sudo ip link set veth10 netns $PID6

#Host 1
sudo nsenter -t $PID1 -n ip addr add 10.0.0.1/24 dev veth1
sudo nsenter -t $PID1 -n ip link set dev veth1 address 00:00:00:00:01:01
sudo nsenter -t $PID1 -n ip link set veth1 up

#SWITCH 1
#Port 1
sudo nsenter -t $PID2 -n ip addr add 10.0.0.2/24 dev veth2
sudo nsenter -t $PID2 -n ip link set dev veth2 address 00:00:00:00:01:02
sudo nsenter -t $PID2 -n ip link set veth2 up
#Port 2
sudo nsenter -t $PID2 -n ip addr add 10.0.2.1/24 dev veth3
sudo nsenter -t $PID2 -n ip link set dev veth3 address 00:00:00:00:02:01
sudo nsenter -t $PID2 -n ip link set veth3 up

#SWITCH 2
#Port 1
sudo nsenter -t $PID3 -n ip addr add 10.0.2.2/24 dev veth4
sudo nsenter -t $PID3 -n ip link set dev veth4 address 00:00:00:00:02:02
sudo nsenter -t $PID3 -n ip link set veth4 up
#Port 2
sudo nsenter -t $PID3 -n ip addr add 10.0.3.1/24 dev veth5
sudo nsenter -t $PID3 -n ip link set dev veth5 address 00:00:00:00:03:01
sudo nsenter -t $PID3 -n ip link set veth5 up

#SWITCH 3
#Port 1
sudo nsenter -t $PID4 -n ip addr add 10.0.3.2/24 dev veth6
sudo nsenter -t $PID4 -n ip link set dev veth6 address 00:00:00:00:03:02
sudo nsenter -t $PID4 -n ip link set veth6 up
#Port 2
sudo nsenter -t $PID4 -n ip addr add 10.0.4.1/24 dev veth7
sudo nsenter -t $PID4 -n ip link set dev veth7 address 00:00:00:00:04:01
sudo nsenter -t $PID4 -n ip link set veth7 up

#SWITCH 5
#Port 1
sudo nsenter -t $PID5 -n ip addr add 10.0.4.2/24 dev veth8
sudo nsenter -t $PID5 -n ip link set dev veth8 address 00:00:00:00:04:02
sudo nsenter -t $PID5 -n ip link set veth8 up
#Port 2
sudo nsenter -t $PID5 -n ip addr add 10.0.5.1/24 dev veth9
sudo nsenter -t $PID5 -n ip link set dev veth9 address 00:00:00:00:05:01
sudo nsenter -t $PID5 -n ip link set veth9 up

#Host2
sudo nsenter -t $PID6 -n ip addr add 10.0.5.2/24 dev veth10
sudo nsenter -t $PID6 -n ip link set dev veth10 address 00:00:00:00:05:02
sudo nsenter -t $PID6 -n ip link set veth10 up

#Configurar as rotas no switch para utilizarem o switch na mesma rede como gateway
docker exec h1 route add -net 10.0.5.2 netmask 255.255.255.255 gw 10.0.0.2
docker exec h2 route add -net 10.0.0.1 netmask 255.255.255.255 gw 10.0.5.1

#Inicia o BMV2
docker exec sw1 sh -c 'nohup simple_switch --thrift-port 50001 -i 1@veth2 -i 2@veth3 standard.json &'
docker exec sw2 sh -c 'nohup simple_switch --thrift-port 50002 -i 1@veth4 -i 2@veth5 standard.json &'
docker exec sw3 sh -c 'nohup simple_switch --thrift-port 50003 -i 1@veth6 -i 2@veth7 standard.json &'
docker exec sw4 sh -c 'nohup simple_switch --thrift-port 50004 -i 1@veth8 -i 2@veth9 standard.json &'

#Configura as entradas no BMV2 - Válido apenas para cenários CP
docker exec sw1 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.0.1  =>  00:00:00:00:01:01 1" | simple_switch_CLI --thrift-port 50001'
docker exec sw1 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.5.2 =>  00:00:00:00:05:02 2" | simple_switch_CLI --thrift-port 50001'
docker exec sw2 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.0.1  =>  00:00:00:00:01:01 1" | simple_switch_CLI --thrift-port 50002'
docker exec sw2 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.5.2 =>  00:00:00:00:05:02 2" | simple_switch_CLI --thrift-port 50002'
docker exec sw3 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.0.1  =>  00:00:00:00:01:01 1" | simple_switch_CLI --thrift-port 50003'
docker exec sw3 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.5.2 =>  00:00:00:00:05:02 2" | simple_switch_CLI --thrift-port 50003'
docker exec sw4 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.0.1  =>  00:00:00:00:01:01 1" | simple_switch_CLI --thrift-port 50004'
docker exec sw4 sh -c 'echo "table_add MyIngress.ipv4_lpm ipv4_forward 10.0.5.2 =>  00:00:00:00:05:02 2" | simple_switch_CLI --thrift-port 50004'


