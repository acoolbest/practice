#!/bin/sh 
#===================== 
#version
#=====================
CONFIG_JSON=../application/config.json

SOFTWARE_VERSION=$(awk '/releaseVersion/{print $3}' ${CONFIG_JSON})
SOFTWARE_VERSION=${SOFTWARE_VERSION#*\"}
SOFTWARE_VERSION=${SOFTWARE_VERSION%%\"*}

SOFTWARE_RELEASE_DATE=$(awk '/releaseDate/{print $3}' ${CONFIG_JSON})
SOFTWARE_RELEASE_DATE=${SOFTWARE_RELEASE_DATE#*\"}
SOFTWARE_RELEASE_DATE=${SOFTWARE_RELEASE_DATE%%\"*}

echo "*************************************************************************"
echo "*\tProduct: charge\t\t\t\t\t\t\t*"
echo "*\tVersion: v$SOFTWARE_VERSION\t\t\t\t\t\t\t*"
echo "*\tCopyright: Chang Chong Technology (Bei Jing) Co., Ltd.\t\t*"
echo "*\tRelease date: $SOFTWARE_RELEASE_DATE\t\t\t\t\t*"
echo "*************************************************************************"
