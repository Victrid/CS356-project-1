.PHONY: build push push_p push_modules clean
build:
	cd ./jni
	ndk-build
	
push: build avd.lock
	adb root
	adb shell "rm -rf /data/misc/osprj/*"
	adb shell "mkdir -p /data/misc/osprj"
	adb push ./libs/armeabi/* /data/misc/osprj
	adb shell "ls /data/misc/osprj"
	
	
push_p: build avd.lock
	echo "Error not implemented"

push_modules: ptree.ko avd.lock
	adb root
	adb shell "rm -rf /data/misc/osko/*"
	adb shell "mkdir -p /data/misc/osko"
	adb push *.ko /data/misc/osko
	adb shell "ls /data/misc/osko"

%.ko:%
	make -C $<
	cp $</$@ ./

clean:
	rm -f *.ko
	rm -rf libs obj
