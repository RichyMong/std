/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "protos/rpc.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using etcdserverpb::KV;

class EtcdClient {
 public:
  EtcdClient(std::shared_ptr<Channel> channel)
      : stub_(KV::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string get(const std::string& user) {
    using etcdserverpb::RangeRequest;
    using etcdserverpb::RangeResponse;

    // Data we are sending to the server.
    RangeRequest request;
    request.set_key(user);

    // Container for the data we expect from the server.
    RangeResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->Range(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      assert(reply.kvs_size() == 1);
      return reply.kvs(0).value();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<KV::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  EtcdClient etcd(grpc::CreateChannel(
      "localhost:2379", grpc::InsecureChannelCredentials()));
  std::cout << "Value received: " << etcd.get("/mine/front/sh/info.xml") << std::endl;

  return 0;
}
