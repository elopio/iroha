/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <numeric>

#include "ametsuchi/impl/peer_query_wsv.hpp"
#include "ametsuchi/wsv_query.hpp"
#include "builders/protobuf/common_objects/proto_peer_builder.hpp"

namespace iroha {
  namespace ametsuchi {

    PeerQueryWsv::PeerQueryWsv(std::shared_ptr<WsvQuery> wsv)
        : wsv_(std::move(wsv)) {}

    boost::optional<std::vector<PeerQueryWsv::wPeer>>
    PeerQueryWsv::getLedgerPeers() {
      return wsv_->getPeers() | [](const auto &peers) {
        return std::accumulate(
            peers.begin(),
            peers.end(),
            std::vector<PeerQueryWsv::wPeer>{},
            [](auto &vec, const auto &peer) {
              shared_model::proto::PeerBuilder builder;

              auto key =
                  shared_model::crypto::PublicKey(peer.pubkey.to_string());
              auto tmp = builder.address(peer.address).pubkey(key).build();

              vec.emplace_back(tmp.copy());
              return vec;
            });
      };
    }

  }  // namespace ametsuchi
}  // namespace iroha
