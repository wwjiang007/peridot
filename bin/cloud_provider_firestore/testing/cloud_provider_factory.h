// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PERIDOT_BIN_CLOUD_PROVIDER_FIRESTORE_TESTING_CLOUD_PROVIDER_FACTORY_H_
#define PERIDOT_BIN_CLOUD_PROVIDER_FIRESTORE_TESTING_CLOUD_PROVIDER_FACTORY_H_

#include <fuchsia/ledger/cloud/cpp/fidl.h>
#include <fuchsia/ledger/cloud/firestore/cpp/fidl.h>
#include <lib/async-loop/cpp/loop.h>
#include <lib/component/cpp/startup_context.h>
#include <lib/fidl/cpp/binding_set.h>
#include <lib/fxl/memory/ref_ptr.h>
#include <lib/network_wrapper/network_wrapper_impl.h>

#include "peridot/bin/cloud_provider_firestore/include/types.h"
#include "peridot/lib/firebase_auth/testing/credentials.h"
#include "peridot/lib/firebase_auth/testing/service_account_token_provider.h"
#include "peridot/lib/rng/random.h"
#include "peridot/lib/rng/system_random.h"

namespace cloud_provider_firestore {

// Factory for real Firestore cloud provider binaries backed by fake token
// provider.
//
// This is used for end-to-end testing, including the validation test suite for
// the cloud provider.
class CloudProviderFactory {
 public:
  // Opaque container for user id.
  class UserId {
   public:
    UserId(const UserId& user_id);
    UserId(UserId&& user_id);
    UserId& operator=(const UserId& user_id);
    UserId& operator=(UserId&& user_id);

    static UserId New();

    const std::string& user_id() { return user_id_; }

   private:
    UserId();

    std::string user_id_;
  };

  CloudProviderFactory(
      component::StartupContext* startup_context, rng::Random* random,
      std::string api_key,
      std::unique_ptr<service_account::Credentials> credentials);
  ~CloudProviderFactory();

  void Init();

  void MakeCloudProvider(
      UserId user_id,
      fidl::InterfaceRequest<cloud_provider::CloudProvider> request);

  void MakeTokenProvider(
      UserId user_id,
      fidl::InterfaceRequest<fuchsia::modular::auth::TokenProvider> request);

 private:
  class TokenProviderContainer;
  component::StartupContext* const startup_context_;
  rng::Random* const random_;
  const std::string api_key_;
  std::unique_ptr<service_account::Credentials> credentials_;

  // Loop on which the token manager runs.
  async::Loop services_loop_;

  callback::AutoCleanableSet<TokenProviderContainer> token_providers_;

  fuchsia::sys::ComponentControllerPtr cloud_provider_controller_;
  FactoryPtr cloud_provider_factory_;

  FXL_DISALLOW_COPY_AND_ASSIGN(CloudProviderFactory);
};

}  // namespace cloud_provider_firestore

#endif  // PERIDOT_BIN_CLOUD_PROVIDER_FIRESTORE_TESTING_CLOUD_PROVIDER_FACTORY_H_
