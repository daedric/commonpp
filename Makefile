all:
	${MAKE} -C build all

clean:
	${MAKE} -C build clean

cmake:
	rm -rf build
	mkdir build
	cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ..

.PHONY: cmake

package:
	${MAKE} -C build package

test:
	${MAKE} -C build test

re : cmake all
