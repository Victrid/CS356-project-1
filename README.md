# CS356: OS Project 1: Android Process Tree

## Build and run

First edit the android.source.sh with its internal instructions, and 
`source ./android.source.sh` to have the correct environment variables
and PATH.

With the Makefile in the root folder, you may use these commands to build:

Build all:

```
make
make build_all
```

Build the module or executable in the folder

```
make {folder name}
```

If you want to check the code with AVD please 
-    create an AVD and run `./run-sim.sh` separately.
- or create an avd.lock file in the directory to indicate AVD is created.

Then you may use these commands:

 To run the testrun, which would compose testrun contents

```
make testrun
```

Push executable and modules to DEST_FOLDERS defined in the Makefile

```
make push_all
make push_{folder name without extensions}
```

load/unload the module

```
make load_{module folder name without extensions}
make unload_{module folder name without extensions}
```

