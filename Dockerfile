
#
#
# Basic sample docker commands.
#
#
# Create Volune
# docker volume create vol
#
# Build image
# docker build -f Dockerfile . -t arch_test
#
# Create Container
# docker run -it --memory 32gb --cpus=4 --init -d --name arch_test --restart unless-stopped -p 2022:22 --mount type=bind,source=%cd%/ws,target=/ws  --mount type=volume,source=vol,target=/wsVolume  arch_test  /bin/bash
# docker run -it --init -d --name arch_test --restart unless-stopped -p 22:22 --mount type=bind,source=%cd%/ws,target=/ws  --mount type=volume,source=vol,target=/wsVolume  arch_test  /bin/bash
#
# Enter to a running contianer
# docker exec -it --user dev arch_test bash
# docker exec -it arch_test bash
#
#
#
# Basic Docker Operation
# docker restart arch_test
# docker start arch_test
# docker stop arch_test
# docker container rm arch_test
#
# Clear docker
# docker system prune -af
# docker volume prune -af
#
# docker images -a
# docker container ls -a
# docker volume ls
#
#
# echo . && docker images -a && echo . && docker container ls -a && echo . && docker volume ls && echo .
# docker start arch_test  &&  docker exec -it arch_test /bin/bash
# docker stop arch_test && docker system prune -af && docker volume prune -af
# docker update --restart unless-stopped
#
#



FROM archlinux:latest
LABEL name="ArchDevEnv" vendor="cangursu" version="0.08"

RUN pacman -Syyu --noconfirm  && \
    pacman -S --noconfirm base-devel gdb gcc ninja autoconf && \
    pacman -S --noconfirm cmake clang llvm && \
    pacman -S --noconfirm git ccache openssl openssh && \
    pacman -S --noconfirm unzip xz bzip2 && \
    pacman -S --noconfirm curl wget tmux doxygen graphviz && \
    pacman -S --noconfirm vi ntp vim zsh tree htop fzf less && \
    pacman -S --noconfirm python3


RUN groupadd -g 10001 dev && \
    useradd --create-home -s /usr/bin/bash -u 10000 -g dev dev && \
    echo 'dev:dev' | chpasswd && \
    echo 'root:root' | chpasswd && \
    mkdir -p /wsVolume/ && chown -R dev:dev /wsVolume/ && \
    mkdir -p /wsLocal/ && chown -R dev:dev /wsLocal/ && \
    mkdir -p /ws3rd && cd /ws3rd  && \
    ssh-keygen -A && \
    sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    echo "******************** rapidjson ********************" && \
    rm -rf rapidjson && \
    git clone https://github.com/Tencent/rapidjson.git && \
    mkdir -p rapidjson/build && \
    cd rapidjson/build && \
    cmake .. -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF && \
    make -s -j && make install && \
    cd ../.. && \
    echo "******************** googletest ********************" && \
    rm -rf googletest && \
    git clone https://github.com/google/googletest.git && \
    mkdir -p googletest/build && \
    cd  googletest/build && \
    cmake .. && make -s -j && make install && \
    cd ../..  && \
    echo "******************** valgrind ********************" && \
    mkdir -p valgrind && cd valgrind  && \
    wget https://sourceware.org/pub/valgrind/valgrind-3.24.0.tar.bz2 && \
    tar xvf valgrind-3.24.0.tar.bz2 && \
    cd valgrind-3.24.0  && \
    ./configure  && \
    make -s -j && make install && \
    cd ../..  && \
    ` echo "******************** boost ********************" &&` \
    ` rm -f download  && rm -rf boost_1_82_0 &&` \
    ` wget -c https://sourceforge.net/projects/boost/files/boost/1.82.0/boost_1_82_0.tar.gz &&` \
    ` tar -xf ./boost_1_82_0.tar.gz &&` \
    ` cd boost_1_82_0 &&` \
    ` ./bootstrap.sh && ./b2 --quiet install &&` \
    ` cd ../  &&` \
    echo "********************  ********************"



RUN echo "******************** setup ~/.bashrc ********************" && \
    echo "" >> ~/.bashrc && \
    echo "#********************" >> ~/.bashrc && \
    echo "" >> ~/.bashrc && \
    echo "alias l='ls -lahF'" >> ~/.bashrc  && \
    echo "" >> ~/.bashrc && \
    echo 'eval "$(fzf --bash)"'  >> ~/.bashrc  && \
    echo "" >> ~/.bashrc && \
    echo "git config --system credential.helper store"  >> ~/.bashrc && \
    echo "/usr/bin/sshd" >> ~/.bashrc  && \
    echo && \
    echo "dev ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers && \
    echo 



WORKDIR /ws
CMD ["/bin/bash"]
#ENTRYPOINT ["/usr/bin/sshd"]



