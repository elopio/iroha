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

#include <algorithm>
#include <random>

#include "ametsuchi/peer_query.hpp"
#include "consensus/yac/cluster_order.hpp"
#include "consensus/yac/impl/peer_orderer_impl.hpp"
#include "consensus/yac/yac_hash_provider.hpp"
#include "interfaces/common_objects/peer.hpp"

namespace iroha {
  namespace consensus {
    namespace yac {

      template <class NewModel,
                class OldModel = typename NewModel::OldModelType>
      std::vector<OldModel> toOldVector(
          const std::vector<std::shared_ptr<NewModel>> &vec) {
        return std::accumulate(
            vec.begin(),
            vec.end(),
            std::vector<OldModel>{},
            [](auto &out, const auto &item) {
              auto ptr = std::unique_ptr<OldModel>(item->makeOldModel());
              out.emplace_back(*ptr);
              return out;
            });
      };

      PeerOrdererImpl::PeerOrdererImpl(
          std::shared_ptr<ametsuchi::PeerQuery> peer_query)
          : query_(std::move(peer_query)) {}

      nonstd::optional<ClusterOrdering> PeerOrdererImpl::getInitialOrdering() {
        return query_->getLedgerPeers() | [](const auto &peers) {
          auto prs = toOldVector(peers);
          return ClusterOrdering::create(prs);
        };
      }

      nonstd::optional<ClusterOrdering> PeerOrdererImpl::getOrdering(
          const YacHash &hash) {
        return query_->getLedgerPeers() | [&hash](auto peers) {
          auto prs = toOldVector(peers);
          std::seed_seq seed(hash.block_hash.begin(), hash.block_hash.end());
          std::default_random_engine gen(seed);
          std::shuffle(prs.begin(), prs.end(), gen);
          return ClusterOrdering::create(prs);
        };
      }
    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha
