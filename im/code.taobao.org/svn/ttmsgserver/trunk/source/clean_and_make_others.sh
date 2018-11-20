echo "#ifndef __VERSION_H__" > base/version.h
echo "#define __VERSION_H__" >> base/version.h
echo "#define VERSION \"$1\"" >> base/version.h
echo "#endif" >> base/version.h

if [ ! -d lib ]
then
    mkdir lib
fi

cd base
make clean
cmake .
make
if [ $? -eq 0 ]; then
    echo "make base successed";
else
    echo "make base failed";
    exit;
fi
if [ -f libbase.a ]
then
    cp libbase.a ../lib/
fi

cd ../slog
make clean
cmake .
make
if [ $? -eq 0 ]; then
    echo "make slog successed";
else
    echo "make slog failed";
    exit;
fi

mkdir ../base/slog/lib
cp slog_api.h ../base/slog/
cp libslog.so ../base/slog/lib/
cp -a lib/liblog4cxx* ../base/slog/lib/

cp ../libsecurity/lib/unix/libsecurity.a ../base/security/lib/linux

cd ../tools
make
if [ $? -eq 0 ]; then
    echo "make tools successed";
else
    echo "make tools failed";
    exit;
fi