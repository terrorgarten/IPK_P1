#!/bin/sh
echo "$(tput setaf 1)MAKING"
make

echo $"$(tput setaf 1) launching on port 1500 $(tput sgr0)"
./hinfosvc 1500 &>server_output.log &

echo -e "$(tput setaf 1) Getting hostname:$(tput sgr0)"
GET http://localhost:1500/hostname

echo -e "$(tput setaf 1) Getting load:$(tput sgr0)"
GET http://localhost:1500/load

echo -e "\n$(tput setaf 1) Getting CPU name:$(tput sgr0)"
GET http://localhost:1500/cpu-name

echo "$(tput setaf 1) Killing the server$(tput sgr0)"
pkill -f "hinfosvc"

echo "$(tput setaf 1) relaunching$(tput sgr0)"
./hinfosvc 1500 &>server_output.log &

echo -e "$(tput setaf 1) Getting hostname (2):$(tput sgr0)"
GET http://localhost:1500/hostname

echo -e "$(tput setaf 1) Getting load (2):$(tput sgr0)"
GET http://localhost:1500/load

echo -e "\n$(tput setaf 1) Getting CPU name (2):$(tput sgr0)"
GET http://localhost:1500/cpu-name

echo "$(tput setaf 1) Killing the server (2)$(tput sgr0)"

