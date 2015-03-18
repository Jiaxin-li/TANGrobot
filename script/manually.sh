#!/bin/bash 
echo "manully control."
cd /opt/odv/bin
xterm -e ./supercomponent --cid=111 & xterm -e ./cockpit --cid=111 & xterm -e ./vehicle --cid=111 --freq=10
