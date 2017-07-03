cfssl print-defaults config > ca-config.json
cfssl print-defaults csr > ca-csr.json

cfssl gencert -initca ca-csr.json | cfssljson -bare ca -

cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json \
    -profile=server server.json | cfssljson -bare server

cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json \
    -profile=client client.json | cfssljson -bare client

# start server with client auth enabled
./etcd --cert-file self-certs/server.pem --key-file self-certs/server-key.pem \
    --advertise-client-urls https://localhost:2379 \
    --listen-client-urls https://0.0.0.0:2379 \
    --ca-file self-certs/ca.pem --client-cert-auth=true

./etcdctl --endpoints=localhost:2379 --cacert=self-certs/ca.pem \
    --cert=self-certs/client.pem --key=self-certs/client-key.pem \
    --user="mds:mengfanke" get /waipan/mds/marketinfo.xml
