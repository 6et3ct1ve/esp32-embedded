# syntax=docker/dockerfile:1

# Ubuntu 24.04 LTS (noble), pinned by digest: tags get republished,
# a digest always resolves to the same image.
# Update:
#   docker pull ubuntu:noble
#   docker inspect --format='{{index .RepoDigests 0}}' ubuntu:noble
FROM ubuntu:noble@sha256:2198e4c65076e9cb0b3d695219df667c0e3b0072da64ca3c2648aca17dcecaf5

# Keep apt from waiting for interactive prompts during build.
ARG DEBIAN_FRONTEND="noninteractive"

# Install packages from apt-packages.in (one per line, # for comments).
# --no-install-recommends and cache cleanup keep the layer small;
# locale-gen enables en_US.UTF-8 for correct non-ASCII output.

COPY apt-packages.in /tmp/apt.in
RUN apt-get update && \
  grep -vE '^\s*#' /tmp/apt.in | xargs apt-get install --no-install-recommends -y && \
  rm -rf /var/lib/apt/lists/* && apt-get clean -y && rm /tmp/apt.in && \
  sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
  locale-gen

# UTC timezone and UTF-8 locale for consistent logs and non-ASCII output.
ENV TZ=Etc/UTC
ENV LC_ALL=en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en

# gitleaks secret scanner: pinned release, checksum verified before install.
ARG GITLEAKS_VERSION=8.30.1
ARG GITLEAKS_SHA256=551f6fc83ea457d62a0d98237cbad105af8d557003051f41f3e7ca7b3f2470eb
RUN curl -fsSL -o /tmp/gitleaks.tar.gz \
      "https://github.com/gitleaks/gitleaks/releases/download/v${GITLEAKS_VERSION}/gitleaks_${GITLEAKS_VERSION}_linux_x64.tar.gz" && \
    echo "${GITLEAKS_SHA256}  /tmp/gitleaks.tar.gz" | sha256sum -c - && \
    tar -xzf /tmp/gitleaks.tar.gz -C /usr/local/bin gitleaks && \
    rm /tmp/gitleaks.tar.gz

# Container user comes from devcontainer.json (host $USER locally,
# `runner` in CI) so bind-mounted files keep matching ownership.
ARG CONTAINER_USER=developer

# Drop the default `ubuntu` user (UID 1000) so devcontainer UID remapping
# can give our user the host UID; otherwise bind-mounted files look foreign.
# The `if` keeps the build working if a future base image has no `ubuntu`.
RUN if id ubuntu >/dev/null 2>&1; then \
      userdel -r ubuntu && groupdel ubuntu 2>/dev/null || true; \
    fi && \
    useradd -s /bin/bash -m ${CONTAINER_USER}

# Everything below, including VS Code server processes, runs as the
# unprivileged user, not root.
USER ${CONTAINER_USER}

# PlatformIO and pre-commit in a user venv (Ubuntu blocks system-wide pip, PEP 668).
ARG PLATFORMIO_VERSION=6.2.0
ARG PRE_COMMIT_VERSION=4.7.0
RUN python3 -m venv ~/.venv && \
    ~/.venv/bin/pip install --no-cache-dir platformio==${PLATFORMIO_VERSION} pre-commit==${PRE_COMMIT_VERSION}
ENV PATH="/home/${CONTAINER_USER}/.venv/bin:${PATH}"

