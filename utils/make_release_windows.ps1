echo "Creating release for Windows"

$RELEASE_OUTPUT_DIR="release"
$SRC="src"
$BUILD_DIR="build"
$CMAKE_FLAGS="-DRPCLIB_BUILD_TESTS=OFF"

Remove-Item -Force -Recurse $RELEASE_OUTPUT_DIR 
Remove-Item -Force -Recurse $SRC

mkdir $RELEASE_OUTPUT_DIR
pushd $RELEASE_OUTPUT_DIR

git clone --depth=1 --branch=renaming https://github.com/rpclib/rpclib.git $SRC

$archs = @("x86"; "x64")
$configs = @("Debug"; "Release")
$runtimes = @("Static"; "Dynamic")

$runtime_suffixes = @{
	"DymamicRelease"="MD"; 
	"DynamicDebug"="MDd"; 
	"StaticRelease"="MT"; 
	"StaticDebug"="MTd"
}

$arch_generators = @{
	"x86"="Visual Studio 14 2015";
	"x64"="Visual Studio 14 2015 Win64"
}

$extra_cmake_flags = @{
	"MT"="-DRPCLIB_MSVC_STATIC_RUNTIME=ON";
	"MTd"="-DRPCLIB_MSVC_STATIC_RUNTIME=ON"
}

function build_config($arch, $config, $runtime) {
	echo "Building $arch $runtime $config..."
	$special_build_dir = "build_$arch_$runtime".ToLower()
	mkdir $special_build_dir
	pushd $special_build_dir
	cmake ../$SRC -G "Visual Studio 14 2015 Win64" $CMAKE_FLAGS
	cmake --build . --config Release
	popd
}

build_config "x86" "Release" "Dynamic"

popd

foreach ($arch in $archs) {
	foreach ($config in $configs) {
		foreach ($runtime in $runtimes) {

		}
	}
}

