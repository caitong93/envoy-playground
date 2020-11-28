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
