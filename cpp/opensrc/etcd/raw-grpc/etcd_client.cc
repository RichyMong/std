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
using etcdserverpb::Auth;

namespace etcd {

class AuthClient {
 public:
  AuthClient(std::shared_ptr<Channel> channel)
      : auth_stub_(Auth::NewStub(channel)) {}

  bool auth(const std::string& user, const std::string& passwd) {
    using etcdserverpb::AuthenticateRequest;
    using etcdserverpb::AuthenticateResponse;

    // Data we are sending to the server.
    AuthenticateRequest request;
    request.set_name(user);
    request.set_password(passwd);

    // Container for the data we expect from the server.
    AuthenticateResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = auth_stub_->Authenticate(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      token_ = reply.token();
      std::cout << "token is: " << token_ << std::endl;
      return true;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return false;
    }
  }

  const std::string& token() const { return token_; }

 private:
  std::unique_ptr<Auth::Stub> auth_stub_;
  std::string token_;
};

class Client {
 public:
  Client(std::shared_ptr<Channel> channel, const std::string &token)
      : kv_stub_(KV::NewStub(channel)), creds_(grpc::AccessTokenCredentials(token)) {}

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
    context.set_credentials(creds_);

    // The actual RPC.
    Status status = kv_stub_->Range(&context, request, &reply);

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
  std::unique_ptr<KV::Stub> kv_stub_;
  std::shared_ptr<grpc::CallCredentials> creds_;
};

}

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  etcd::AuthClient auth_client(grpc::CreateChannel(
      "202.104.236.84:2379", grpc::InsecureChannelCredentials()));
  if (auth_client.auth("mds", "mengfanke")) {
      auto token_cred = grpc::AccessTokenCredentials(auth_client.token());
      auto cred = grpc::CompositeChannelCredentials(grpc::InsecureChannelCredentials(),
              token_cred);
      etcd::Client client(grpc::CreateChannel("202.104.236.84:2379",
                  grpc::InsecureChannelCredentials()), auth_client.token());
      std::cout << "Value received: " << client.get("/mine/front/sh/info.xml") << std::endl;
  }

  return 0;
}
