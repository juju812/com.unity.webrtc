@echo off

if not exist depot_tools (
  git clone --depth 1 https://chromium.googlesource.com/chromium/tools/depot_tools.git
)

set COMMAND_DIR=%~dp0
set PATH=%cd%\depot_tools;%PATH%
set WEBRTC_VERSION=4183
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set CPPFLAGS=/WX-
set GYP_GENERATORS=ninja,msvs-ninja
set GYP_MSVS_VERSION=2019
set OUTPUT_DIR=out
set ARTIFACTS_DIR=%cd%\artifacts
set PYPI_URL=https://artifactory.prd.it.unity3d.com/artifactory/api/pypi/pypi/simple
set vs2019_install=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community

if not exist src (
  call fetch.bat --nohooks webrtc
  cd src
  call git.bat config --system core.longpaths true
  call git.bat checkout  refs/echo otes/branch-heads/%WEBRTC_VERSION%
  cd ..
  call gclient.bat sync -f
)

echo  add jsoncpp
patch -N "src\BUILD.gn" --binary < "%COMMAND_DIR%\patches\add_jsoncpp.patch"

echo  install pywin32
call "%cd%\depot_tools\bootstrap-2@3_8_10_chromium_17_bin\python\bin\python.exe" ^
  -m pip install pywin32 --upgrade

mkdir "%ARTIFACTS_DIR%\lib"

setlocal enabledelayedexpansion

for %%i in (x64) do (
  mkdir "%ARTIFACTS_DIR%/lib/%%i"
  for %%j in (true false) do (

    echo  generate ninja for release
    call gn.bat gen %OUTPUT_DIR% --root="src" ^
      --args="is_debug=%%j is_clang=false target_cpu=\"%%i\" rtc_include_tests=false rtc_build_examples=false proprietary_codecs=true rtc_use_h264=true ffmpeg_branding=\"Chrome\" treat_warnings_as_errors=false symbol_level=0 enable_iterator_debugging=false"

    echo  build
    ninja.exe -C %OUTPUT_DIR%

    set filename=
    if true==%%j (
      set filename=webrtcd.lib
    ) else (
      set filename=webrtc.lib
    )

    echo  copy static library for release build
    copy "%OUTPUT_DIR%\obj\webrtc.lib" "%ARTIFACTS_DIR%\lib\%%i\!filename!"
  )
)

endlocal

echo  fix error when generate license
patch -N "%cd%\src\tools_webrtc\libs\generate_licenses.py" < ^
  "%COMMAND_DIR%\patches\generate_licenses.patch"

echo  generate license
call python.bat "%cd%\src\tools_webrtc\libs\generate_licenses.py" ^
  --target //:default %OUTPUT_DIR% %OUTPUT_DIR%

echo  unescape license
powershell -File "%COMMAND_DIR%\Unescape.ps1" "%OUTPUT_DIR%\LICENSE.md"

echo  copy header
xcopy src\*.h "%ARTIFACTS_DIR%\include" /C /S /I /F /H /y

echo  copy license
copy "%OUTPUT_DIR%\LICENSE.md" "%ARTIFACTS_DIR%"

echo  create zip
cd %ARTIFACTS_DIR%
7z a -tzip webrtc-win.zip *