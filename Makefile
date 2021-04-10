MODULES=$(wildcard *.mdl)
EXECS=$(wildcard *.exc)

.PHONY: build_all clean
build_all: build_exec_all build_module_all

.PHONY: $(EXECS) build_exec_all clean_exec $(patsubst %.exc,clean_exec_%,${EXECS}) 

build_exec_all: | $(EXECS)

clean_exec: | $(patsubst %.exc,clean_exec_%,${EXECS})

$(EXECS): 
	+ndk-build -C $@

$(patsubst %.exc,clean_exec_%,${EXECS}):
	rm -rf ${patsubst clean_exec_%,%.exc,$@}/libs
	rm -rf ${patsubst clean_exec_%,%.exc,$@}/obj

.PHONY: $(MODULES) build_module_all clean_module 
.PHONY: $(patsubst %.mdl,clean_mdl_%,${MODULES})

push_all : | $(patsubst %.mdl,push_%,${MODULES}) $(patsubst %.exc, push_%, ${EXECS})

build_module_all: | $(MODULES)

clean_module: | $(patsubst %.mdl,clean_mdl_%,${MODULES})

$(MODULES):
	+make -C $@

$(patsubst %.mdl,clean_mdl_%,${MODULES}):
	+make -C ${patsubst clean_mdl_%,%.mdl,$@} clean

$(patsubst %.mdl,push_%,${MODULES}): push_% :%.mdl avd.lock
	adb shell "mkdir -p /data/misc/osprj"
	adb push ${patsubst push_%,%.mdl,$@}/*.ko /data/misc/osprj

.PHONY: $(patsubst %.mdl,load_%,${MODULES}) $(patsubst %.mdl, unload_%,${MODULES})

$(patsubst %.mdl,load_%,${MODULES}): load_% : push_%
	if [ -e .$@ ]; then \
	exit -1; \
	fi
	_module_file="$(notdir $(wildcard ${patsubst load_%,%.mdl,$@}/*.ko))"; \
		     adb shell "insmod /data/misc/osprj/$${_module_file}"
	touch .$@

$(patsubst %.mdl, unload_%,${MODULES}): unload_% : .load_%
	_module_file="$(notdir $(wildcard ${patsubst unload_%,%.mdl,$@}/*.ko))"; \
		     adb shell "rmmod /data/misc/osprj/$${_module_file}"
	rm ${patsubst unload_%, .load_%,$@}

.PHONY: $(pathsubst %.exc, push_%,${EXECS}) $(patsubst %.exc, run_%,${EXECS})

$(patsubst %.exc, push_%, ${EXECS}): push_%: %.exc avd.lock
	adb shell "mkdir -p /data/misc/osprj"
	adb push ${patsubst push_%, %.exc, $@}/libs/armeabi/* /data/misc/osprj

$(patsubst %.exc, run_%, ${EXECS}): run_%: push_%
	_exe_file="$(notdir $(wildcard ${patsubst run_%,%.exc,$@}/libs/armeabi/*))"; \
		  adb shell "/data/misc/osprj/$${_exe_file}"

clean: | clean_module clean_exec

