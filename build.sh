mkdir -p build
cd build
if [[ -n $1 ]];then
  OPTIONS=$1
fi
cmake $OPTIONS \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_STATIC=ON -DCMAKE_INSTALL_PREFIX=$HOME/opt/caph ..
make -j4
cd ..
