# get lingeling
wget http://fmv.jku.at/lingeling/lingeling-ayv-86bf266-140429.zip
unzip -o lingeling-ayv-86bf266-140429.zip
mv *.txt code/
rm build.sh
rm -rf lingeling
mv code lingeling

# make lingeling
cd lingeling
./configure.sh
make
cd ..

# make minisat
cd minisat
make
cd ..

# make hordesat
cd hordesat-src
#make
make libs
cd ..
#mv hordesat-src/hordesat .
mv hordesat-src/libhordesat.a .
rm *.zip*
