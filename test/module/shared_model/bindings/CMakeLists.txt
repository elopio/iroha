# Copyright 2017 Soramitsu Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


if (SWIG_PYTHON)
  find_package(PythonInterp 2.7 REQUIRED)
  add_test(NAME python_bindings_test
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/builder-test.py
    WORKING_DIRECTORY $ENV{PYTHONPATH})
  set_tests_properties(python_bindings_test
    PROPERTIES DEPENDS bindings
               ENVIRONMENT "PYTHONPATH=$ENV{PYTHONPATH}")
endif()