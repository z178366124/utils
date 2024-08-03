ROOT_PWD=$(pwd)
cd libyaml
rm -rf install
mkdir install
cd install
cmake -DCMAKE_INSTALL_PREFIX=/home/ruci/Cproject/utils/libs .. 
make
make install


cd $ROOT_PWD/libcyaml
echo $ROOT_PWD
make BUILDDIR=/home/ruci/Cproject/utils/libs/libcyaml

rm -rf install
mkidr install
cd install
cmake ..
make