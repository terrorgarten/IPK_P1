#!/bin/sh
echo "$(tput setaf 1)MAKING"
make

echo "Launching on port 1500"
./hinfosvc 1500 &>server_output.log &

echo -e "Getting hostname:"
GET http://localhost:1500/hostname

echo -e "Getting load:"
GET http://localhost:1500/load

echo -e "\nGetting CPU name:"
GET http://localhost:1500/cpu-name

echo "Killing the server"
pkill -f "hinfosvc"

echo "relaunching"
./hinfosvc 1500 &>server_output.log &

echo -e "Getting hostname (2):"
GET http://localhost:1500/hostname

echo -e "Getting load (2):"
GET http://localhost:1500/load

echo -e "Getting CPU name (2):"
GET http://localhost:1500/cpu-name

echo "Killing the server.."

