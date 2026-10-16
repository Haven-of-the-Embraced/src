FROM ubuntu:22.04 AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends gcc make libc6-dev libcrypt-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build/src
COPY src/ ./
RUN make

FROM ubuntu:22.04

RUN apt-get update \
    && apt-get install -y --no-install-recommends libcrypt1 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/haven/src
COPY --from=build /build/src/rom ./rom
COPY docker/entrypoint.sh /usr/local/bin/haven-entrypoint
RUN sed -i 's/\r$//' /usr/local/bin/haven-entrypoint \
    && chmod +x /usr/local/bin/haven-entrypoint

EXPOSE 4000
ENTRYPOINT ["/usr/local/bin/haven-entrypoint"]
CMD ["4000"]