#!/bin/bash

LD_PRELOAD=/opt/envoy/socket_hook.so exec /usr/local/bin/envoy --concurrency 2 --drain-time-s 2 --parent-shutdown-time-s 3 --restart-epoch $RESTART_EPOCH -c /opt/envoy/envoy.yaml -l debug 