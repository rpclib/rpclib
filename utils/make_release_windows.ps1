echo "Creating release for Windows"

$RELEASE_OUTPUT_DIR="release"
$SRC="src"
$BUILD_DIR="build"
$CMAKE_FLAGS="-DRPCLIB_BUILD_TESTS=OFF"

Remove-Item -Force -Recurse $RELEASE_OUTPUT_DIR 

mkdir $RELEASE_OUTPUT_DIR
pushd $RELEASE_OUTPUT_DIR

git clone --depth=1 --branch=dev https://github.com/rpclib/rpclib.git $SRC

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
	"MD"="";
	"MDd"="";
	"MT"="-DRPCLIB_MSVC_STATIC_RUNTIME=ON";
	"MTd"="-DRPCLIB_MSVC_STATIC_RUNTIME=ON"
}

function build_config($arch, $runtime, $config) {
	echo "Building $arch $runtime $config..."
	$special_build_dir = "build_$arch$runtime$config".ToLower()
	echo "Build directory: $special_build_dir"
	mkdir $special_build_dir
	pushd $special_build_dir
	
	$suffix = $runtime_suffixes["$runtime$config"]
	$gen = $arch_generators[$arch]
	$flags = $extra_cmake_flags[$suffix]
	$suffix_lower = $suffix.ToLower()
	
	cmake ../$SRC -G $gen $CMAKE_FLAGS $flags -DRPCLIB_NAME_SUFFIX="$suffix_lower"
	cmake --build . --config $config
	
	popd
}

build_config "x64" "Static" "Debug"

foreach ($arch in $archs) {
	foreach ($config in $configs) {
		foreach ($runtime in $runtimes) {
			#build_config $arch $runtime $config
		}
	}
}

popd

