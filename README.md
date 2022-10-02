# Mobile-MSM

## Build

### Android app

1. Download Android Studio and install the Android API 32 platform, NDK, and Android SDK Platform tools through the SDK Manager. **Please note that, NDK version should better be `r24` or above `r24`.**
2. Open `mobileMSM` in Android Studio.
3. Use Android Studio's device manager to select the target device.
4. Install the app on that device using the Run > Run 'app' menu action.

### C library

Prebuilt libraries are available in this repository as `Mobile-MSM-open/mobileMSM/app/src/main/jniLibs/armeabi-v7a/` named `libyyteam-mobileMSM.so`.

It is welcome to recompile the library by yourself.

1. First you need download the NDK, which should better be **`r24` or above `r24`.**
2. Set up `ANDROID_NDK_ROOT` environment using `export ANDROID_NDK_ROOT=/path/to/android-ndk-r24/`.
3. Then compile the library.
    ```bash
    cd Mobile-MSM-open/src/
    make libyyteam-mobileMSM.so
    ```
4. Copy `Mobile-MSM-open/src/libyyteam-mobileMSM.so` to `Mobile-MSM-open/mobileMSM/app/src/main/jniLibs/armeabi-v7a/libyyteam-mobileMSM.so`.
5. Rebuild and install the app as described above.

### Rust library

> Note that, this rust library is only used for generate random points and scalars test vectors. We took the code in [zprize-mobile-harness/src/lib.rs](https://github.com/celo-org/zprize-mobile-harness/blob/main/src/lib.rs) as reference and modified it to meet our situation.

Prebuilt rust libraries are available in this repository as `Mobile-MSM-open/mobileMSM/app/src/main/jniLibs/armeabi-v7a/` named `libcelo_zprize.so`. This library is only used to generate random files which are stored as `/data/data/com.example.zprize/files/points` and `/data/data/com.example.zprize/files/scalars`.

It is welcome to recompile the library by yourself.

1. Download the [cross](https://github.com/cross-rs/cross) crate.
2. Then compile the library.
    ```bash
    cd Mobile-MSM-open/rust_src/
    cross build --target armv7-linux-androideabi --release
    ```
3. Copy `Mobile-MSM-open/rust_src/src/target/armv7-linux-androideabi/release/libcelo_zprize.so` to `Mobile-MSM-open/mobileMSM/app/src/main/jniLibs/armeabi-v7a/libcelo_zprize.so`.
4. Rebuild and install the app as described above.

## Usage

We use the same app interface as [zprize-mobile-harness](https://github.com/celo-org/zprize-mobile-harness) does.

> #iterations per vector: This input value determins the number of iteration for each test vector pair (base point and scalar pair).
> 
> #vectors to generate randomly: When using `PRESS TO RUN USING RANDOM ELEMENTS` button, this value is a must and tells our program to generate THIS number of vector pairs.
> 
> #elems as power of 2: When using `PRESS TO RUN USING RANDOM ELEMENTS` button, this value is a must and tells our program the size of each vector pair we are about to genrate. E.g., we input 16, which means that each points vector contains 2^16 points, each scalars vector contains 2^16 scalars.

1. `PRESS TO RUN FROM TEST VECTOR FILE` button: This button will call the program to run MSM on the test files (`points` and `scalars`) we prepared in `Mobile-MSM-open/mobileMSM/app/src/main/assets/`.
2. `PRESS TO RUN USING RANDOM ELEMENTS` button: This button will call the program to run MSM on the random test file according to the input values. And the test file will be stored in `/data/data/com.example.mobilemsm/files/` on the target device.  

### Test File Notice

We put two kinds of test vector files in `Mobile-MSM-open/mobileMSM/app/src/main/assets/normal-test` and `Mobile-MSM-open/mobileMSM/app/src/main/assets/zero-points-test` respectively.

1. `normal-test`: This folder contains the normal random test vectors that we generated as example for the benchmark. The files in this folder contain two 2^16 length test vectors. The file format is consistent with official test vector files format. The `points` and `scalars` files that already stored in `Mobile-MSM-open/mobileMSM/app/src/main/assets/` are the same as files stored in `Mobile-MSM-open/mobileMSM/app/src/main/assets/normal-test`.
2. `zero-points-test`: This folder contains the test vectors with all zero base points as example for the correctness test. (For this purpose, one could replace the  `points` and `scalars` in `Mobile-MSM-open/mobileMSM/app/src/main/assets/` with files stored in `zero-points-test`). The files in the folder contain one 2^8 length test vector. The file format is different from the `normal-test` one. We take the zero base point file format as rust ark code generated, and **PLEASE NOTE THAT** our file reader in the benchmark method **now only supports THIS file format for recognizing the zero base point** (which won't influence the normal vectors test for sure).

### Curve Point Definition

1. We define **Infinity Affine Point** as `(x, y) = (0, 1).` (Both over **Short Weierstrass curves of BLS12-377 G1** and **Twisted Edwards curves of BLS12-377 G1**).
2. We define **Infinity Projective Point** as `(x, y, z, t) = (0, 1, 1, 0)` over **Twisted Edwards curves of BLS12-377 G1**. Moreover, any Projective Point `(0, y, z, 0)` is an **Infinity Point** as long as it meets the condition: `y == z && y != 0`.

## File Format

1. Our implementation takes **uncompressed** vector files as inputs. Besides, the input points is Affine points over **Short Weierstrass curves of BLS12-377 G1**.
2. The `result.txt` file shows results of MSM on each vector (with readable encoding). The result points is also Affine points over **Short Weierstrass curves of BLS12-377 G1**, which helps one to check the correctness of our implementation.
3. The `resulttime.txt` file shows the running time of MSM for each iteration on each vector.

## Structure
Core MSM algorithm is in `src/msm.c`.
Main function is in `main.cpp` and function `run_yyteam_msm_benchmark` gives the benchmark interface.

## Main Optimization

- MSM is implemented by bucket method and parallelized. 
    - We use multiple cores to run each window sum for bucket method parallelly.
    - The final sum up operation is still running with one thread.
- The basic operations and simple field operaions of BigInt are optimized by aarch64 assembly instructions. Note that, the simple field operation means the operations that need at most one modulus reduction (e.g., 384-bit addition operation over the field.).
    - We take the advantages of aarch64 ISA for fast BigInt operations. We use 64-bit arithmetic instructions, `mul` and `umulh` for example, to construct 384-bit BigInt basic operations.
    - We also use the aarch64 ISA for optimizing field operations over **BLS12-377 G1 base field**. We implement fast simple field operation using aarch64 ISA.
- The complex field operations over **BLS12-377 G1 base field** are optimized with assembly code and Montgomery method. Note that, the complex field operation means the operations that need lots of reduction steps (i.e., 384-bit multiplication and square operation over the field.).
    - We implement fast Montgomery Multiplication operation (also include Square operation) with Word-by-Word Montgomery Multiplication algorithm.
    - We also use aarch64 ISA to provide an assembly version.
- The elliptic curve point operations are implemented over **Twisted Edwards Curve**, which directly decrease the basic operations a elliptic curve point operation needs.
    - Firstly, we transform all the Affine points over **Short Weierstrass curve** to the Affine points over **Twisted Edwards Curve**.
    - Instead of transforming all **Twisted Edwards Curve's** Affine points to Projective points, we implement mix point addition method over **Twisted Edwards Curve**.
    - We also provide point doubling and point addition over **Twisted Edwards Curve's** Projective Coordinates.
    - We will finally transform the result back to the **Short Weierstrass Curve's** Affine point.
    
- All operations is regulated to the Montgomery domain.
    - We will transform all the data to Montgomery domain, and than do the rest operations for MSM.
    - We will finally transform the result back to the noraml domain.

## Benchmark Notice

> The following things we mentioned is what we found in our own test cases, maybe it could help the judging team for a better test.

- The screen should be kept unlocked, otherwise the whole power of the device won't be released.
- Besides, for the random vectors test part, the random generator we use (as official baseline does) is a little bit slow. If one wants to generate a huge random vectors for test using `PRESS TO RUN USING RANDOM ELEMENTS` button in the app, the benchmark result may be influenced by the generating part because **it may raise the device's temperature**. Generate the random files outside, move them to the `Mobile-MSM-open/mobileMSM/app/src/main/assets/` folder, and then use `PRESS TO RUN FROM TEST VECTOR FILE` button would be a better way.

## Time Cost Calculation

We **only take the MSM part itself into consideration** when calculate the time cost.

Specifically, we do not count any data transform overheads. 

## Result

The final result is about 1000ms.
