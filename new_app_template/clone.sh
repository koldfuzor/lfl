#!/bin/bash
# $Id: clone.sh 1264 2011-11-24 08:43:37Z justin $

if [ $# -ne 3 ]; then echo "Usage: $0 <org> <package> <binary>"; echo "Eg: $0 com.lucidfusionlabs FusionViewer fv"; exit 1; fi

ORGNAME=$1
PKGNAME=$2
BINNAME=$3

TEMPLATEDIR=`dirname $0`
if [ "$TEMPLATEDIR" = "" -o "$TEMPLATEDIR" = "." ]; then echo "Can't run from new_app_template directory"; exit 1; fi

TEMPLATEFILES=`find $TEMPLATEDIR/* | grep -v "/\." | grep -v "/clone.sh" | grep -v "/pkg/" | grep -v "/assets/" | grep -v "README.txt"`

echo "lflpub/new_app_template/clone.sh: Cloning new app"
echo "Domain: $DOMNAME"
echo "Package: $PKGNAME"
echo "Directory/Binary: $BINNAME"

mkdir $BINNAME || { echo "CLONE FAILED: mkdir $BINNAME"; exit 1; }

cp -R $TEMPLATEDIR/pkg $BINNAME
cp -R $TEMPLATEDIR/assets $BINNAME
find $BINNAME -name .svn | xargs rm -rf

echo "$ORGNAME" > $BINNAME/pkg/OrgName.txt
echo "$PKGNAME" > $BINNAME/pkg/PkgName.txt
echo "$BINNAME" > $BINNAME/pkg/BinName.txt

for f in $TEMPLATEFILES; do
    t=`echo $f | sed -e s,"$TEMPLATEDIR","$BINNAME", -e s,"new_app_template","$BINNAME",g`

    let len=${#f}-4
    suffix=${f:$len}

    if [ -d $f ]; then
        mkdir $t
    elif [ "$suffix" = ".png" ]; then
        cp $f $t
    else
        cat $f | $BINNAME/pkg/pkgsedpipe.sh $BINNAME/pkg > $t
    fi
done

echo "lflpub/new_app_template/clone.sh: Successfully cloned new_app_template $BINNAME"

