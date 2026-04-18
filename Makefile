
all:
	cmake -B build -DCMAKE_BUILD_TYPE=Release -DCXX_STANDARD=23
	cmake --build build -t install
