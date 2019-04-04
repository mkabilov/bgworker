# bgworker
Experiments with PostgreSQL background worker written in golang 


## Build
```
make
make install
```

## Load
add "go_background_worker" to `shared_preload_libraries` in your postgresql config file:
```
shared_preload_libraries = 'go_background_worker'    # (change requires restart)
```

restart postgresql

## Check the logs
you should see following message appearing every 5 seconds:
```
LOG:  Go: Hello world
```
