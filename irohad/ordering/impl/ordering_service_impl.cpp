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

#include "ordering/impl/ordering_service_impl.hpp"
#include "ametsuchi/ordering_service_persistent_state.hpp"
#include "backend/protobuf/transaction.hpp"
#include "builders/protobuf/proposal.hpp"
#include "logger/logger.hpp"

namespace iroha {
  namespace ordering {
    OrderingServiceImpl::OrderingServiceImpl(
        std::shared_ptr<ametsuchi::PeerQuery> wsv,
        size_t max_size,
        size_t delay_milliseconds,
        std::shared_ptr<network::OrderingServiceTransport> transport,
        std::shared_ptr<ametsuchi::OrderingServicePersistentState>
            persistent_state)
        : wsv_(wsv),
          max_size_(max_size),
          delay_milliseconds_(delay_milliseconds),
          transport_(transport),
          persistent_state_(persistent_state) {
      updateTimer();
      log_ = logger::log("OrderingServiceImpl");

      // restore state of ordering service from persistent storage
      proposal_height = persistent_state_->loadProposalHeight().value();
    }

    void OrderingServiceImpl::onTransaction(
        std::shared_ptr<shared_model::interface::Transaction> transaction) {
      queue_.push(transaction);
      log_->info("Queue size is {}", queue_.unsafe_size());

      if (queue_.unsafe_size() >= max_size_) {
        handle.unsubscribe();
        updateTimer();
      }
    }

    void OrderingServiceImpl::generateProposal() {
      std::vector<shared_model::proto::Transaction> fetched_txs;
        log_->info("Start proposal generation");
      for (std::shared_ptr<shared_model::interface::Transaction> tx;
           fetched_txs.size() < max_size_ and queue_.try_pop(tx);) {
        fetched_txs.emplace_back(
            std::move(static_cast<shared_model::proto::Transaction &>(*tx)));
      }

      auto proposal = std::make_unique<shared_model::proto::Proposal>(
          shared_model::proto::ProposalBuilder()
              .height(proposal_height++)
              .createdTime(iroha::time::now())
              .transactions(fetched_txs)
              .build());

      // Save proposal height to the persistent storage.
      // In case of restart it reloads state.
      persistent_state_->saveProposalHeight(proposal_height);

      publishProposal(std::move(proposal));
    }

    void OrderingServiceImpl::publishProposal(
        std::unique_ptr<shared_model::interface::Proposal> proposal) {
      std::vector<std::string> peers;

      auto lst = wsv_->getLedgerPeers().value();
      for (const auto &peer : lst) {
        peers.push_back(peer->address());
      }
      transport_->publishProposal(std::move(proposal), peers);
    }

    void OrderingServiceImpl::updateTimer() {
      if (not queue_.empty()) {
        this->generateProposal();
      }
      timer = rxcpp::observable<>::timer(
          std::chrono::milliseconds(delay_milliseconds_));
      handle = timer.subscribe_on(rxcpp::observe_on_new_thread())
                   .subscribe([this](auto) { this->updateTimer(); });
    }

    OrderingServiceImpl::~OrderingServiceImpl() {
      handle.unsubscribe();
    }
  }  // namespace ordering
}  // namespace iroha
