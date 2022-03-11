#!/bin/sh
echo "---------MAKING-----------"
make

echo "-------GET TESTS--------"

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

echo "Relaunching"
./hinfosvc 1500 &>server_output.log &

echo -e "Getting hostname (2):"
GET http://localhost:1500/hostname

echo -e "Getting load (2):"
GET http://localhost:1500/load

echo -e "Getting CPU name (2):"
GET http://localhost:1500/cpu-name

echo "-------CURL TESTS--------"
touch curl_cpu-name
touch curl_hostname
touch curl_load

echo "HOSTNAME:"
curl  http://localhost:1500/hostname --verbose --output curl_hostname
cat curl_hostname

echo "LOAD:"
curl  http://localhost:1500/load --verbose --output curl_load
cat curl_load
echo "HOSTNAME:"

curl  http://localhost:1500/cpu-name --verbose --output curl_cpu-name
cat curl_cpu-name

rm curl_cpu-name
rm curl_hostname
rm curl_load

echo "END"