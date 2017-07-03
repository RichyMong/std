#!/usr/bin/env bash

declare -a SERVICES=( 'mds' 'gqs' )

function create_user_as_role
{
    user=$1

    etcdctl --endpoints=localhost:2379 --cacert=certs/ca.crt\
        --cert=certs/client.crt --key=certs/client.key role add $USER

    etcdctl --endpoints=localhost:2379 --cacert=certs/ca.crt\
        --cert=certs/client.crt --key=certs/client.key role grant-permission\
        $USER --prefix=true read /waipan/$USER/ 

    etcdctl --endpoints=localhost:2379 --cacert=certs/ca.crt\
        --cert=certs/client.crt --key=certs/client.key user add $USER

    etcdctl --endpoints=localhost:2379 --cacert=certs/ca.crt\
        --cert=certs/client.crt --key=certs/client.key user passwd $USER

    etcdctl --endpoints=localhost:2379 --cacert=certs/ca.crt\
        --cert=certs/client.crt --key=certs/client.key user grant-role $USER $USER
}

for service in ${SERVICES[@]}; do
    create_user_as_role $service
done
