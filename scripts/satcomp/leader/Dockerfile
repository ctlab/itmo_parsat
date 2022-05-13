################### Use itmo-parsat base (builder) image
FROM itmo-parsat-base AS builder
USER root

################### Extract itmo-parsat in run stage
FROM satcomp-base:leader AS itmo-parsat_liaison

WORKDIR /
# Copy itmo-parsat and solver scripts
COPY --from=builder /itmo-parsat /itmo-parsat
COPY --chown=ecs-user /run_ips.sh /competition
COPY --chown=ecs-user /solver /competition

USER ecs-user
RUN chmod +x /competition/run_ips.sh
RUN chmod +x /competition/solver
