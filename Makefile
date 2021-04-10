MODULES=$(wildcard *.mdl)
EXECS=$(wildcard *.exc)
DEST_FOLDERS=$${DEST_FOLDER}

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
	adb shell "mkdir -p ${DEST_FOLDERS}"
	adb push ${patsubst push_%,%.mdl,$@}/*.ko ${DEST_FOLDERS}

.PHONY: $(patsubst %.mdl,load_%,${MODULES}) $(patsubst %.mdl, unload_%,${MODULES})

$(patsubst %.mdl,load_%,${MODULES}): load_% : push_%
	if [ -e .$@ ]; then \
	exit -1; \
	fi
	_module_file="$(notdir $(wildcard ${patsubst load_%,%.mdl,$@}/*.ko))"; \
		     adb shell "insmod ${DEST_FOLDERS}/$${_module_file}"
	touch .$@

$(patsubst %.mdl, unload_%,${MODULES}): unload_% : .load_%
	_module_file="$(notdir $(wildcard ${patsubst unload_%,%.mdl,$@}/*.ko))"; \
		     adb shell "rmmod ${DEST_FOLDERS}/$${_module_file}"
	rm ${patsubst unload_%, .load_%,$@}

.PHONY: $(pathsubst %.exc, push_%,${EXECS}) $(patsubst %.exc, run_%,${EXECS})

$(patsubst %.exc, push_%, ${EXECS}): push_%: %.exc avd.lock
	adb shell "mkdir -p ${DEST_FOLDERS}"
	adb push ${patsubst push_%, %.exc, $@}/libs/armeabi/* ${DEST_FOLDERS}

$(patsubst %.exc, run_%, ${EXECS}): run_%: push_%
	_exe_file="$(notdir $(wildcard ${patsubst run_%,%.exc,$@}/libs/armeabi/*))"; \
		  adb shell "${DEST_FOLDERS}/$${_exe_file}"

clean: | clean_module clean_exec

testrun: avd.lock push_all
	adb shell "${DEST_FOLDERS}/helloARM"
	adb shell "echo robust ptree without module"
	adb shell "${DEST_FOLDERS}/ptree || true"
	adb shell "echo load module and run again"
	adb shell "insmod ${DEST_FOLDERS}/ptree.ko"
	adb shell "${DEST_FOLDERS}/ptree"
	adb shell "echo robust ptreetest input"
	adb shell "${DEST_FOLDERS}/ptreetest || true"
	adb shell "echo robust ptreetest without module"
	adb shell "rmmod ${DEST_FOLDERS}/ptree.ko"
	adb shell "${DEST_FOLDERS}/ptreetest ${DEST_FOLDERS}/ptree || true"
	adb shell "echo load module and run again"
	adb shell "insmod ${DEST_FOLDERS}/ptree.ko"
	adb shell "${DEST_FOLDERS}/ptreetest ${DEST_FOLDERS}/ptree"
	adb shell "echo burger buddies problem"
	adb shell "echo robust input"
	adb shell "${DEST_FOLDERS}/burger || true"
	adb shell "${DEST_FOLDERS}/burger 1 2 4 -1 || true"
	adb shell "echo sample 1"
	adb shell "${DEST_FOLDERS}/burger 1 1 1 5"
	adb shell "echo sample 2"
	adb shell "${DEST_FOLDERS}/burger 10 3 5 30"



