cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_STATIC=ON -DCMAKE_INSTALL_PREFIX=$HOME/opt/caph ..
make -j4
cd ..
