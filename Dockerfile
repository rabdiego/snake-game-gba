FROM archlinux:latest

RUN pacman -Sy --noconfirm \
    curl \
    wget \
    git \
    make \
    base-devel \
    gnupg

ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=/opt/devkitpro/devkitARM
ENV PATH=$PATH:$DEVKITARM/bin:$DEVKITPRO/tools/bin

RUN pacman-key --init && \
    pacman-key --recv BC26F752D25B92CE272E0F44F7FD5492264BB9D0 --keyserver keyserver.ubuntu.com && \
    pacman-key --lsign BC26F752D25B92CE272E0F44F7FD5492264BB9D0

RUN pacman -U --noconfirm https://pkg.devkitpro.org/devkitpro-keyring.pkg.tar.zst || true && \
    pacman-key --populate devkitpro

RUN echo -e "\n[dkp-libs]\nServer = https://pkg.devkitpro.org/packages\n\n[dkp-linux]\nServer = https://pkg.devkitpro.org/packages/linux/\$arch/" >> /etc/pacman.conf

RUN pacman -Syu --noconfirm

RUN pacman -S --noconfirm devkitARM gba-dev

WORKDIR /project
COPY . /project

CMD ["make"]

