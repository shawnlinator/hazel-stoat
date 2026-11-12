FROM debian:trixie

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc-14 g++-14 lld libcurl4-openssl-dev liblzma-dev python3 curl ca-certificates git \
    && rm -rf /var/lib/apt/lists/*

COPY . /src
WORKDIR /src

# Fetches mongo's bazelisk, which then downloads the AOT bazel binary pinned in
# .bazelversion via the BAZELISK_BASE_URL in .bazeliskrc (public S3 bucket).
RUN python3 buildscripts/install_bazel.py

ENV PATH="$HOME/.local/bin:$PATH"

# --config=native_toolchain opts out of the hermetic Linux action container
# (which needs podman on the host) and compiles with the host GCC 14.
RUN CC=gcc-14 CXX=g++-14 bazel build --config=native_toolchain --compiler_type=gcc \
    --jobs=16 --disable_warnings_as_errors=True install-mongod
