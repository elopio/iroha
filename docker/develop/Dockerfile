FROM ubuntu:16.04

# number of concurrent threads during build
# usage: docker build --build-arg PARALLELISM=8 -t name/name .
ARG PARALLELISM=1

ENV IROHA_HOME /opt/iroha
ENV IROHA_BUILD /opt/iroha/build

RUN apt-get update; \
    apt-get -y upgrade; \
    apt-get -y --no-install-recommends install apt-utils software-properties-common; \
    apt-get -y clean

# add git repository
RUN add-apt-repository -y ppa:git-core/ppa; \
    apt-get update

RUN apt-get -y --no-install-recommends install build-essential python-software-properties \
        automake libtool \
        # dev dependencies
        libssl-dev zlib1g-dev libc6-dbg golang \
        # CircleCI dependencies
        git ssh tar gzip ca-certificates python3 python3-pip python3-setuptools \
        # code coverage
        lcov \
        # other
        wget curl file unzip gdb iputils-ping vim ccache \
        gcovr cppcheck doxygen graphviz graphviz-dev; \
    apt-get -y clean

RUN pip3 install --upgrade pip; \
    pip3 install flask requests

# install cmake 3.5.1
RUN git clone https://gitlab.kitware.com/cmake/cmake.git /tmp/cmake; \
    (cd /tmp/cmake ; git checkout 64130a7e793483e24c1d68bdd234f81d5edb2d51); \
    (cd /tmp/cmake ; /tmp/cmake/bootstrap --parallel=${PARALLELISM} --enable-ccache); \
    make -j${PARALLELISM} -C /tmp/cmake; \
    make -C /tmp/cmake install; \
    rm -rf /tmp/cmake

# install boost 1.65.1
RUN git clone https://github.com/boostorg/boost /tmp/boost; \
    (cd /tmp/boost ; git checkout 436ad1dfcfc7e0246141beddd11c8a4e9c10b146); \
    (cd /tmp/boost ; git submodule init); \
    (cd /tmp/boost ; git submodule update --recursive -j ${PARALLELISM}); \
    (cd /tmp/boost ; /tmp/boost/bootstrap.sh --with-libraries=system,filesystem); \
    (cd /tmp/boost ; /tmp/boost/b2 headers); \
    (cd /tmp/boost ; /tmp/boost/b2 cxxflags="-std=c++14" -j ${PARALLELISM} install); \
    rm -rf /tmp/boost

# install protobuf
RUN git clone https://github.com/google/protobuf /tmp/protobuf; \
    (cd /tmp/protobuf ; git checkout 80a37e0782d2d702d52234b62dd4b9ec74fd2c95); \
    cmake -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=ON -H/tmp/protobuf/cmake -B/tmp/protobuf/.build; \
    cmake --build /tmp/protobuf/.build --target install -- -j${PARALLELISM}; \
    ldconfig; \
    rm -rf /tmp/protobuf

# install gflags
RUN git clone https://github.com/gflags/gflags /tmp/gflags; \
    (cd /tmp/gflags ; git checkout f8a0efe03aa69b3336d8e228b37d4ccb17324b88); \
    cmake -H/tmp/gflags -B/tmp/gflags/build; \
    cmake --build /tmp/gflags/build --target install -- -j${PARALLELISM}; \
    rm -rf /tmp/gflags

# install c-ares
RUN git clone https://github.com/c-ares/c-ares /tmp/c-ares; \
    (cd /tmp/c-ares ; git checkout 3be1924221e1326df520f8498d704a5c4c8d0cce); \
    cmake -H/tmp/c-ares -B/tmp/c-ares/build; \
    cmake --build /tmp/c-ares/build --target install -- -j${PARALLELISM}; \
    ldconfig; \
    rm -rf /tmp/c-ares
    
# install grpc
RUN git clone https://github.com/grpc/grpc /tmp/grpc; \
    (cd /tmp/grpc ; git checkout bfcbad3b86c7912968dc8e64f2121c920dad4dfb); \
    (cd /tmp/grpc ; git submodule update --init third_party/benchmark); \
    cmake -DgRPC_ZLIB_PROVIDER=package -DgRPC_CARES_PROVIDER=package -DgRPC_SSL_PROVIDER=package \
        -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_GFLAGS_PROVIDER=package -DBUILD_SHARED_LIBS=ON -H/tmp/grpc -B/tmp/grpc/.build; \
    cmake --build /tmp/grpc/.build --target install -- -j${PARALLELISM}; \
    ldconfig; \
    rm -rf /tmp/grpc

# install gtest
RUN git clone https://github.com/google/googletest /tmp/googletest; \
    (cd /tmp/googletest ; git checkout ec44c6c1675c25b9827aacd08c02433cccde7780); \
    cmake -H/tmp/googletest -B/tmp/googletest/build; \
    cmake --build /tmp/googletest/build --target install -- -j${PARALLELISM}; \
    rm -rf /tmp/googletest

# install spdlog v0.16.3
RUN git clone https://github.com/gabime/spdlog /tmp/spdlog; \
    (cd /tmp/spdlog ; git checkout ccd675a286f457068ee8c823f8207f13c2325b26); \
    cmake -DSPDLOG_BUILD_TESTING=OFF -H/tmp/spdlog -B/tmp/spdlog/build; \
    cmake --build /tmp/spdlog/build --target install; \
    rm -rf /tmp/spdlog

# install rxcpp
RUN git clone https://github.com/Reactive-Extensions/RxCpp /tmp/RxCpp; \
    (cd /tmp/RxCpp ; git checkout 1b2e0589f19cb34d8cd58803677701dcf2161876); \
    cmake -H/tmp/RxCpp -B/tmp/RxCpp/build; \
    cmake --build /tmp/RxCpp/build --target install; \
    rm -rf /tmp/RxCpp

# install rapidjson
RUN git clone https://github.com/miloyip/rapidjson /tmp/rapidjson; \
    (cd /tmp/rapidjson ; git checkout f54b0e47a08782a6131cc3d60f94d038fa6e0a51); \
    cmake -DRAPIDJSON_BUILD_EXAMPLES=OFF -H/tmp/rapidjson -B/tmp/rapidjson/build; \
    cmake --build /tmp/rapidjson/build --target install; \
    rm -rf /tmp/rapidjson

# install optional
RUN git clone https://github.com/martinmoene/optional-lite /tmp/optional-lite; \
    (cd /tmp/optional-lite ; git checkout a0ddabb8b52e1eaaf0dd1515bb85698b747528e4); \
    cp -r /tmp/optional-lite/include/nonstd /usr/local/include; \
    rm -rf /tmp/optional-lite

# install libpq
RUN git clone https://git.postgresql.org/git/postgresql.git /tmp/postgresql; \
    (cd /tmp/postgresql ; git checkout 029386ccbddd0a33d481b94e511f5219b03e6636); \
    (cd /tmp/postgresql ; /tmp/postgresql/configure --without-readline --prefix=/usr/local); \
    make -j${PARALLELISM} -C /tmp/postgresql/src/bin/pg_config; \
    make -j${PARALLELISM} -C /tmp/postgresql/src/interfaces/libpq; \
    make -C /tmp/postgresql/src/bin/pg_config install; \
    make -C /tmp/postgresql/src/interfaces/libpq install; \
    make -C /tmp/postgresql/src/include install; \
    rm -rf /tmp/postgresql

# install pqxx
RUN git clone https://github.com/jtv/libpqxx /tmp/libpqxx; \
    (cd /tmp/libpqxx ; git checkout 5b17abce5ac2b1a2f8278718405b7ade8bb30ae9); \
    (cd /tmp/libpqxx ; /tmp/libpqxx/configure --disable-documentation --with-pic); \
    make -j${PARALLELISM} -C /tmp/libpqxx; \
    make -C /tmp/libpqxx install; \
    rm -rf /tmp/libpqxx

# install tbb
RUN git clone https://github.com/01org/tbb /tmp/tbb; \
    (cd /tmp/tbb ; git checkout eb6336ad29450f2a64af5123ca1b9429ff6bc11d); \
    make -j${PARALLELISM} -C /tmp/tbb tbb_build_prefix=build; \
    cp /tmp/tbb/build/build_debug/*.so* /usr/local/lib; \
    cp /tmp/tbb/build/build_release/*.so* /usr/local/lib; \
    cp -r /tmp/tbb/include/* /usr/local/include; \
    ldconfig; \
    rm -rf /tmp/tbb

# install docker
ENV DOCKER_VERSION=17.06.0-ce
RUN curl -L -o /tmp/docker-${DOCKER_VERSION}.tgz https://download.docker.com/linux/static/stable/x86_64/docker-${DOCKER_VERSION}.tgz; \
    tar -xz -C /tmp -f /tmp/docker-${DOCKER_VERSION}.tgz; \
    mv /tmp/docker/* /usr/bin; \
    rm /tmp/docker-${DOCKER_VERSION}.tgz; \
    rm -rf /tmp/docker

# install sonar cli
ENV SONAR_CLI_VERSION=3.0.3.778
RUN mkdir -p /opt/sonar; \
    curl -L -o /tmp/sonar.zip https://sonarsource.bintray.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-${SONAR_CLI_VERSION}-linux.zip; \
    unzip -o -d /tmp/sonar-scanner /tmp/sonar.zip; \
    mv /tmp/sonar-scanner/sonar-scanner-${SONAR_CLI_VERSION}-linux /opt/sonar/scanner; \
    ln -s -f /opt/sonar/scanner/bin/sonar-scanner /usr/local/bin/sonar-scanner; \
    rm -rf /tmp/sonar*

# install sonar build wrapper
RUN curl -L -o /tmp/sonar-build-wrapper.zip https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip; \
    unzip -o -d /tmp/sonar-build-wrapper /tmp/sonar-build-wrapper.zip; \
    mv /tmp/sonar-build-wrapper/build-wrapper-linux-x86 /opt/sonar/build-wrapper; \
    ln -s -f /opt/sonar/build-wrapper/build-wrapper-linux-x86-64 /usr/local/bin/sonar-build-wrapper; \
    rm -rf /tmp/sonar*

# install latest valgrind
RUN git clone git://sourceware.org/git/valgrind.git /tmp/valgrind; \
    (cd /tmp/valgrind ; /tmp/valgrind/autogen.sh); \
    (cd /tmp/valgrind ; /tmp/valgrind/configure); \
    make -j${PARALLELISM} -C /tmp/valgrind; \
    make -C /tmp/valgrind install; \
    rm -rf /tmp/valgrind

# install ed25519
RUN git clone git://github.com/hyperledger/iroha-ed25519.git /tmp/ed25519; \
    (cd /tmp/ed25519 ; git checkout e7188b8393dbe5ac54378610d53630bd4a180038); \
    cmake -DCMAKE_BUILD_TYPE=Debug -DTESTING=OFF -H/tmp/ed25519 -B/tmp/ed25519/build; \
    cmake --build /tmp/ed25519/build --target install -- -j${PARALLELISM}; \
    ldconfig; \
    rm -rf /tmp/ed25519

# non-interactive adduser
#   -m = create home dir
#   -s = set default shell
#   iroha-ci = username
#   -u = userid, default for Ubuntu is 1000
#   -U = create a group same as username
#   no password
RUN useradd -ms /bin/bash iroha-ci -u 1000 -U

WORKDIR /opt/iroha
RUN chmod -R 777 /opt/iroha; \
    mkdir -p /tmp/ccache -m 777;

USER iroha-ci
CMD ["/bin/bash"]
