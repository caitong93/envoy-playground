#!/bin/bash

LD_PRELOAD=/opt/envoy/socket_hook.so exec /usr/local/bin/envoy --concurrency 2 --drain-time-s 5 --parent-shutdown-time-s 15 --restart-epoch $RESTART_EPOCH -c /opt/envoy/envoy.yaml -l debug 