mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DHAVE_STD_REGEX=ON -DRUN_HAVE_STD_REGEX=1 ..
make