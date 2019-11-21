mkdir bin
cd bin
rm -rf * 
cd ..

mkdir build 

cd build
rm -rf * 

cmake ..
make -j8

mkdir ../bin
mv main_program ../bin/main_program

#rm -rf * 

cd ../bin

echo " "
echo "Run the program..."
echo " "
../bin/main_program

cd .. # back to the main directory 


