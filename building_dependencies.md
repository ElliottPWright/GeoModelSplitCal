# 0. Environment (LCG 108)
```
cd <YOUR EOS HOME>
unset CMAKE_PREFIX_PATH
unset GeoModel_DIR

source /cvmfs/sft.cern.ch/lcg/views/LCG_108/x86_64-el9-gcc13-opt/setup.sh

export BASEDIR=<YOUR EOS Home>/geomodel_stack
mkdir -p $BASEDIR/{externals,build,install}
cd $BASEDIR/externals
```

# 1. Eigen

```
git clone https://gitlab.com/libeigen/eigen.git
cd eigen

mkdir build install
cd build

cmake .. \
  -DCMAKE_INSTALL_PREFIX=$BASEDIR/externals/eigen/install

make -j$(nproc)
make install

export EIGEN_ROOT=$BASEDIR/externals/eigen/install
cd $BASEDIR/externals
```

# 2. Motif

```
curl -L -o motif.tar.gz \
  https://sourceforge.net/projects/motif/files/Motif%202.3.8%20Source%20Code/motif-2.3.8.tar.gz/download

tar -xzf motif.tar.gz
cd motif-2.3.8

export LCG=/cvmfs/sft.cern.ch/lcg/releases

export CPPFLAGS="-I/usr/include/freetype2"

export LDFLAGS="\
-L$LCG/brotli/1.1.0/x86_64-el9-gcc13-opt/lib \
-L$LCG/freetype/2.13.3-013f6/x86_64-el9-gcc13-opt/lib \
-L$LCG/fontconfig/2.14.2-0c670/x86_64-el9-gcc13-opt/lib \
-lbrotlidec -lbrotlicommon"

./configure \
  --prefix=$BASEDIR/externals/motif/install

make -j4
make install

export MOTIF_ROOT=$BASEDIR/externals/motif/install

# runtime linking (IMPORTANT)
export LD_LIBRARY_PATH=$MOTIF_ROOT/lib:$LD_LIBRARY_PATH

cd $BASEDIR/externals
```

# 3. GeoModel

```
git clone https://gitlab.cern.ch/GeoModelDev/GeoModel.git
cd GeoModel

mkdir build install
cd build

cmake .. \
  -DCMAKE_INSTALL_PREFIX=$BASEDIR/GeoModel/install \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=20 \
  -DGEOMODEL_BUILD_GEOMODELG4=ON \
  -DGEOMODEL_BUILD_GEOMODEL_GUI=OFF \
  -DGEOMODEL_USE_QT=OFF \
  -DMOTIF_INCLUDE_DIR=$MOTIF_ROOT/include \
  -DMOTIF_LIBRARIES=$MOTIF_ROOT/lib/libXm.so

make -j$(nproc)
make install

export GeoModel_DIR=$BASEDIR/GeoModel/install/lib/cmake/GeoModel
export LD_LIBRARY_PATH=$BASEDIR/GeoModel/install/lib:$LD_LIBRARY_PATH

cd $BASEDIR
```

# 4. GeoModelSplitCal

```
cd <YOUR EOS Home directory>
git clone https://github.com/ElliottPWright/GeoModelSplitCal.git
cd GeoModelSplitCal
mkdir build
cd build

export CMAKE_PREFIX_PATH=$BASEDIR/GeoModel/install:$CMAKE_PREFIX_PATH

cmake .. \
  -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH \
  -DMOTIF_INCLUDE_DIR=$MOTIF_ROOT/include \
  -DGeoModelCore_DIR=$GeoModelCore_DIR \
  -DMOTIF_LIBRARIES="$MOTIF_ROOT/lib/libXm.so" \
  -DCMAKE_EXE_LINKER_FLAGS="-lXft"

make -j4
```