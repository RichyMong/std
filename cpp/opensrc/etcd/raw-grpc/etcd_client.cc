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
#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <grpc++/grpc++.h>
#include <cstdlib>
#include <boost/property_tree/xml_parser.hpp>
#include "protos/rpc.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::CompletionQueue;
using grpc::ClientAsyncResponseReader;
using grpc::ClientAsyncReaderWriter;

using etcdserverpb::KV;
using etcdserverpb::Auth;
using etcdserverpb::RangeRequest;
using etcdserverpb::RangeResponse;
using etcdserverpb::WatchRequest;
using etcdserverpb::WatchResponse;

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
        : kv_stub_(KV::NewStub(channel)),
          token_(token),
          rpc_thread_(std::thread(&Client::AsyncCompleteRpc, this))
    {
    }

    std::string get(const std::string& user) {
        RangeRequest request;
        request.set_key(user);

        ClientContext context;
        if (!token_.empty()) {
            context.AddMetadata("token", token_);
        }

        RangeResponse reply;

        auto status = kv_stub_->Range(&context, request, &reply);
        return ProcessGetResponse(status, reply);
    }

    void async_get(const std::string& user) {
        RangeRequest request;
        request.set_key(user);

        AsyncClientCall* call = new AsyncClientCall;
        if (!token_.empty()) {
            call->context.AddMetadata("token", token_);
        }

        call->response_reader = kv_stub_->AsyncRange(&call->context, request, &cq_);
        call->response_reader->Finish(&call->reply, &call->status, (void*)call);
    }

    void Stop() {
        cq_.Shutdown();
        rpc_thread_.join();
    }

private:
    std::string ProcessGetResponse(const Status &status, const RangeResponse &reply) {
        if (status.ok()) {
            if (reply.kvs().size() > 0) {
                std::cout << reply.kvs(0).key() << ": " << reply.kvs(0).value() << std::endl;
                return reply.kvs(0).value();
            } else {
                std::cout << "empty value" << std::endl;
                return std::string();
            }
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        // Block until the next result is available in the completion queue "cq".
        while (cq_.Next(&got_tag, &ok)) {
            // The tag in this example is the memory location of the call object
            AsyncClientCall* call = reinterpret_cast<AsyncClientCall*>(got_tag);

            ProcessGetResponse(call->status, call->reply);

            // Once we're complete, deallocate the call object.
            delete call;
        }
    }

private:
    struct AsyncClientCall {
        RangeResponse reply;
        ClientContext context;
        Status status;
        std::unique_ptr<ClientAsyncResponseReader<RangeResponse>> response_reader;
    };

    std::unique_ptr<KV::Stub> kv_stub_;
    std::unique_ptr<KV::Stub> watch_stub_;
    std::unique_ptr<ClientAsyncReaderWriter<WatchRequest, WatchResponse>> watch_stream_;
    std::string token_;
    CompletionQueue cq_;
    std::thread rpc_thread_;
};

}

std::string
read (const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::in);
	if (file.is_open()) {
		std::stringstream ss;
		ss << file.rdbuf ();

		return ss.str ();
    } else {
        std::cerr << "Failed to open file " << filepath << std::endl;
    }

	return std::string();
}

class Config {
public:
    Config(const std::string &path)
        : ssl_on_ { false },
          auth_on_ { false }
    {
        using boost::property_tree::ptree;
        ptree pt;
        read_xml(path, pt);

        host_ = pt.get<std::string>("xml.host");

        auto ssl = pt.get_child_optional("xml.ssl");
        if (ssl) {
            auto ssl_pt = ssl.value();
            auto on_pt = ssl_pt.get_optional<bool>("on");
            if (on_pt && on_pt.value()) {
                ssl_on_ = true;
                ca_file_ = ssl_pt.get<std::string>("ca-file");
                cert_file_ = ssl_pt.get<std::string>("cert-file");
                key_file_ = ssl_pt.get<std::string>("key-file");
            }
        }

        auto auth = pt.get_child_optional("xml.auth");
        if (auth) {
            auto auth_pt = auth.value();
            auto enable_pt = auth_pt.get_optional<bool>("on");
            if (enable_pt && enable_pt.value()) {
                if (!ssl_on_) {
                    std::cerr << "Warning: auth enabled but ssl disabled" << std::endl;
                }
                auth_on_ = enable_pt.value();
                user_ = auth_pt.get<std::string>("user");
                passwd_ = auth_pt.get<std::string>("passwd");
            }
        }
    }

    const std::string& host() const { return host_; }
    const std::string& ca_file() const { return ca_file_; }
    const std::string& cert_file() const { return cert_file_; }
    const std::string& key_file() const { return key_file_; }
    const std::string& user() const { return user_; }
    const std::string& passwd() const { return passwd_; }
    bool auth_on() const { return auth_on_; }
    bool ssl_on() const { return ssl_on_; }

private:
    std::string host_;
    std::string ca_file_;
    std::string cert_file_;
    std::string key_file_;
    std::string user_;
    std::string passwd_;
    bool        ssl_on_;
    bool        auth_on_;
};

int main(int argc, char** argv) {
    std::string cfg_file = argc > 1 ? argv[1] : "config.xml";

    std::shared_ptr<grpc::ChannelCredentials> creds;
    std::string token;

    Config config(cfg_file);
    if (config.auth_on()) {
        auto root = read(config.ca_file());
        auto key = read(config.key_file());
        auto cert = read(config.cert_file());
        grpc::SslCredentialsOptions opts = { root, key, cert };
        creds = grpc::SslCredentials(opts);
        etcd::AuthClient auth_client(grpc::CreateChannel(config.host(), creds));
        if (auth_client.auth(config.user(), config.passwd())) {
            token = auth_client.token();
        }
    } else {
        creds = grpc::InsecureChannelCredentials();
    }

    etcd::Client client(grpc::CreateChannel(config.host(), creds), token);
    std::cout << "make synchronous request\n";
    std::cout << "make asynchronous request\n";
    client.async_get("/waipan/mds/hz/marketinfo.xml");
    client.async_get("/waipan/mds/hz/marketcode.xml");
    sleep(3);
    client.Stop();

  return 0;
}
