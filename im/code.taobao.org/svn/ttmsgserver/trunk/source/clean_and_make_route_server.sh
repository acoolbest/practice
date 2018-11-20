cd route_server
make clean
cmake .
make
if [ $? -eq 0 ]; then
    echo "make successed";
else
    echo "make failed";
    exit;
fi