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
touch curl_cpu-name curl_hostname curl_load curl_hostnameee curl_invalid

echo "++++++HOSTNAME:"
curl  http://localhost:1500/hostname --verbose --output curl_hostname
echo "~~~~~OUTPUT:"
cat curl_hostname

echo "++++++LOAD:\n"
curl  http://localhost:1500/load --verbose --output curl_load
echo "~~~~~OUTPUT:"
cat curl_load

echo "++++++CPU NAME:"
curl  http://localhost:1500/cpu-name --verbose --output curl_cpu-name
echo "~~~~~OUTPUT:"
cat curl_cpu-name

echo "++++++WRONG REQUEST: /hostnameee "
curl  http://localhost:1500/hostnameee --verbose --output curl_hostnameee
echo "~~~~~OUTPUT:"
cat curl_hostnameee

echo "++++++WRONG REQUEST: /invalid "
curl  http://localhost:1500/invalid --verbose --output curl_invalid
echo "~~~~~OUTPUT:"
cat curl_hostnameee

rm curl_cpu-name curl_hostname curl_load curl_hostnameee curl_invalid



echo "-------WGET TESTS--------"

echo "++++++HOSTNAME:"
wget  http://localhost:1500/hostname
echo "~~~~~OUTPUT:"
cat hostname

echo "++++++LOAD:\n"
wget  http://localhost:1500/load
echo "~~~~~OUTPUT:"
cat load

echo "++++++CPU-NAME:"
wget  http://localhost:1500/cpu-name
echo "~~~~~OUTPUT:"
cat cpu-name

echo "++++++WRONG REQUEST: /hostnameee "
wget  http://localhost:1500/hostnameee
echo "~~~~~OUTPUT:"
cat hostnameee

echo "++++++WRONG REQUEST: /invalid "
wget  http://localhost:1500/invalid
echo "~~~~~OUTPUT:"
cat invalid









echo "\n----------------------END---------------------------"