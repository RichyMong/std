CERT_DIR=certs

etcd --client-cert-auth=true --ca-file $CERT_DIR/ca.crt --cert-file $CERT_DIR/server.crt\
    --key-file $CERT_DIR/server.key --advertise-client-urls https://localhost:2379\
    --listen-client-urls https://0.0.0.0:2379

etcdctl --endpoints=localhost:2379 --cacert=$CERT_DIR/ca.crt\
    --cert=$CERT_DIR/client.crt --key=$CERT_DIR/client.key --user=mds:mds get\
    /waipan/mds/hz/marketinfo.xml
