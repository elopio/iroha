#
# Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
# http://soramitsu.co.jp
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

addtest(test_subscriber_testing test_subscriber_testing.cpp)
target_link_libraries(test_subscriber_testing
    rxcpp
    )

add_library(test_block_generator test_block_generator.cpp)
target_link_libraries(test_block_generator
    model
    )

target_include_directories(test_block_generator PUBLIC ${PROJECT_SOURCE_DIR}/test)


add_library(integration_framework
    integration_framework/integration_test_framework.cpp
    )
target_link_libraries(integration_framework
    application
    raw_block_insertion
    keys_manager
    model_generators
    pb_model_converters
    tbb
    )

target_include_directories(integration_framework PUBLIC ${PROJECT_SOURCE_DIR}/test)
