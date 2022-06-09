update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 13 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11
conan profile update settings.compiler.version=11 default
