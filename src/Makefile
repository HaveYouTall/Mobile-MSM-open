API=30
CROSS_COMPILER=$(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android$(API)-

CC=$(CROSS_COMPILER)clang++
CPP=$(CROSS_COMPILER)clang++
AS=$(CROSS_COMPILER)clang++


CSRCS=$(wildcard ./*.c)
CPPSRCS=$(wildcard ./*.cpp)
ASSRCS=$(wildcard ./*.s)

COBJS=$(CSRCS:.c=.o)
CPPOBJS=$(CPPSRCS:.cpp=.o)
ASOBJS=$(ASSRCS:.s=.o)


INCLUDE=-Iinclude


STATIC_FLAGS = -L $(ANDROID_NDK_ROOT)/platforms/android-$(API)/arch-arm64/usr/lib/ -ldl
LIBDL=$(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/$(API)/libdl.a

# openmp ref: https://www.codegrepper.com/code-examples/cpp/dlopen+failed%3A+library+%22libomp.so%22+not+found
libyyteam-mobileMSM.so: $(COBJS) $(CPPOBJS) $(ASOBJS)
	$(CC) -o $@ $^ $(LIBDL) $(INCLUDE) -fopenmp -static-openmp -static -O3
$(COBJS):%.o:%.c
	$(CC) -c $^ -o $@ $(INCLUDE) -fopenmp -static-openmp -O3 
$(CPPOBJS):%.o:%.cpp
	$(CPP) -c $^ -o $@ $(INCLUDE) -fopenmp -static-openmp -O3 
$(ASOBJS):%.o:%.s
	$(AS) -c $^ -o $@ -O3

.PHONY:clean
clean:
	rm $(COBJS) $(CPPOBJS) $(ASOBJS)

