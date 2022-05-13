################### Use Mallob
FROM itmo-parsat-base AS builder
USER root

################### Extract Mallob in run stage
FROM satcomp-base:worker AS itmo-parsat_liaison

# (why do I need to) Copy itmo-parsat solver
#WORKDIR /
#COPY --from=builder /itmo-parsat /itmo-parsat

COPY --chown=ecs-user /worker /competition
RUN chmod +x /competition/worker
