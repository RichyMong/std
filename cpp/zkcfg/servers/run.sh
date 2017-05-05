#!/usr/bin/env bash

if [ $# -lt 1 ]; then
    echo "${BASH_SOURCE-$0}: <id> ..."
    exit 1
fi

CLASSPATH="/etc/zookeeper/conf:/usr/share/java/jline.jar:/usr/share/java/log4j-1.2.jar:/usr/share/java/xercesImpl.jar:/usr/share/java/xmlParserAPIs.jar:/usr/share/java/netty.jar:/usr/share/java/slf4j-api.jar:/usr/share/java/slf4j-log4j12.jar:/usr/share/java/zookeeper.jar"

for zid in $@; do
    java -cp $CLASSPATH \
        org.apache.zookeeper.server.quorum.QuorumPeerMain zoo${zid}.cfg
done
