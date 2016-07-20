DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CURRENT_DIR=`pwd`
cd $DIR
if [ $1 ]
    then
        echo "changing beacon no"
        BEACON_NO=$1
        cat ../beacon_config.h.template | sed 's/4/'$BEACON_NO'/g' > ../beacon_config.h 
fi
cd ../pca10028/s110/armgcc && make clean
cd $DIR
cd ../pca10028/s110/armgcc && make
cd -
./loadbin.sh

cd $CURRENT_DIR
