cfssl print-defaults config > self-certs/ca-config.json
vim self-certs/ca-config.json
cfssl print-defaults csr > self-certs/ca-csr.json
vim self-certs/ca-csr.json

cd self-certs

cfssl gencert -initca ca-csr.json | cfssljson -bare ca -

cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json\
    -profile=server server.json | cfssljson -bare server

cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json\
    -profile=client client.json | cfssljson -bare client

cd ..

# start server with client auth enabled
etcd  --client-cert-auth=true --trusted-ca-file self-certs/ca.pem\
    --cert-file self-certs/server.pem\
    --key-file self-certs/server-key.pem\
    --advertise-client-urls https://localhost:2379\
    --listen-client-urls https://0.0.0.0:2379

etcdctl --endpoints=localhost:2379 --cacert=self-certs/ca.pem\
    --cert=self-certs/client.pem --key=self-certs/client-key.pem\
    --user="mds:mds" get /waipan/mds/marketinfo.xml
