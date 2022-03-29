#!/usr/bin/env sh

# opk name to project folder name
BIN_NAME=$(basename ${PWD%/*})
OPK_NAME="$BIN_NAME.opk"
echo ${BIN_NAME}
echo ${OPK_NAME}

# strip executable
cp ../a.out ${BIN_NAME}
#${STRIP} ${BIN_NAME}

# create opk
rm -f ${OPK_NAME}
mksquashfs default.funkey-s.desktop ${BIN_NAME} icon.png ${OPK_NAME} -all-root -no-xattrs -noappend -no-exports