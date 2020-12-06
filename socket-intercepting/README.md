# This example demostrates how to intercetpt socket() call

## The `LD_PRELOAD` trick

* http://www.goldsborough.me/c/low-level/kernel/2016/08/29/16-48-53-the_-ld_preload-_trick/
* https://fishi.devtail.io/weblog/2015/01/25/intercepting-hooking-function-calls-shared-c-libraries/

## Run

Build `socket_hook.c`

```
gcc -Wall -fPIC -shared socket_hook.c -o socket_hook.so
```

Run envoy as container

```
docker run --rm -it -v `pwd`:/opt/envoy --entrypoint=/bin/bash envoyproxy/envoy:v1.16-latest
```

Install python inside container

```
root@521f9bfdf0ed:~# apt update -y && apt install python
```
Start envoy using [hot-restarter.py](https://www.envoyproxy.io/docs/envoy/latest/intro/arch_overview/operations/hot_restart)

```
root@521f9bfdf0ed:~# cd /opt/envoy
root@521f9bfdf0ed:/opt/envoy# ./hot-restarter.py start-envoy.sh
```

Now you can see `socket() call intercepted` messages in logs

```
[2020-11-28 09:47:18.347][1062][debug][upstream] [source/common/upstream/cluster_manager_impl.cc:1198] membership update for TLS cluster service_google added 1 removed 0
socket() call intercepted
```
## Reproduce https://github.com/envoyproxy/envoy/issues/14295

This example will show an issue that child notifies parent to drain listeners before child listeners are ready, which happens when `reuse_port` is enabled. In `socket_hook2.c`, we will block the first `socket()` call for 5s for each thread.

First compile `socket_hook2.c`

```
gcc -Wall -fPIC -shared socket_hook2.c -o socket_hook.so
```

Lauch Envoy according to the steps above. Then send `HUP` signal to hot-restarter.py.
Envoy starts with `--drain-time-s 2 --parent-shutdown-time-s 3` (defined in `start-envoy.sh`).

Now let's analyse logs:

child start workers, `socket()` calls are blocked

```
[2020-12-06 09:13:20.804][546][info][config] [source/server/listener_manager_impl.cc:888] all dependencies initialized. starting workers
[2020-12-06 09:13:20.804][546][debug][config] [source/server/listener_manager_impl.cc:899] starting worker 0
[2020-12-06 09:13:20.804][546][debug][config] [source/server/listener_manager_impl.cc:899] starting worker 1
[2020-12-06 09:13:20.805][555][debug][main] [source/server/worker_impl.cc:124] worker entering dispatch loop
[2020-12-06 09:13:20.805][546][warning][main] [source/server/server.cc:565] there is no configured limit to the number of allowed active connections. Set a limit via the runtime key overload.global_downstream_max_connections
[2020-12-06 09:13:20.805][554][debug][main] [source/server/worker_impl.cc:124] worker entering dispatch loop
[2020-12-06 09:13:20.805][555][debug][upstream] [source/common/upstream/cluster_manager_impl.cc:1044] adding TLS initial cluster service_google
[2020-12-06 09:13:20.805][554][debug][upstream] [source/common/upstream/cluster_manager_impl.cc:1044] adding TLS initial cluster service_google
[2020-12-06 09:13:20.805][555][debug][upstream] [source/common/upstream/cluster_manager_impl.cc:1198] membership update for TLS cluster service_google added 1 removed 0
[2020-12-06 09:13:20.805][554][debug][upstream] [source/common/upstream/cluster_manager_impl.cc:1198] membership update for TLS cluster service_google added 1 removed 0
[555]socket() call intercepted
[554]socket() call intercepted
[554]sleep in first socket() call
[2020-12-06 09:13:20.805][557][debug][grpc] [source/common/grpc/google_async_client_impl.cc:49] completionThread running
[2020-12-06 09:13:20.805][556][debug][grpc] [source/common/grpc/google_async_client_impl.cc:49] completionThread running
[555]sleep in first socket() call
```

Parent start shutting down

```
[2020-12-06 09:13:23.807][546][info][main] [source/server/drain_manager_impl.cc:70] shutting down parent after drain
```

In child, socket calls return

```
[554]sleep end
[555]sleep end
[2020-12-06 09:13:25.805][554][debug][config] [source/server/listener_impl.cc:107] Create listen socket for listener listener_0 on address 0.0.0.0:10000
[2020-12-06 09:13:25.805][554][debug][config] [source/server/listener_impl.cc:117] listener_0: Setting socket options succeeded
[2020-12-06 09:13:25.806][555][debug][config] [source/server/listener_impl.cc:107] Create listen socket for listener listener_0 on address 0.0.0.0:10000
[2020-12-06 09:13:25.806][555][debug][config] [source/server/listener_impl.cc:117] listener_0: Setting socket options succeeded
```