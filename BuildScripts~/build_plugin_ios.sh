#!/bin/bash -eu

export LIBWEBRTC_DOWNLOAD_URL=https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M89/webrtc-ios.zip
export SOLUTION_DIR=$(pwd)/Plugin~
export WEBRTC_FRAMEWORK_DIR=$(pwd)/Runtime/Plugins/iOS

# Install cmake
export HOMEBREW_NO_AUTO_UPDATE=1
brew install cmake

# Download webrtc 
curl -L $LIBWEBRTC_DOWNLOAD_URL > webrtc.zip
unzip -d $SOLUTION_DIR/webrtc webrtc.zip 

# Build webrtc Unity plugin 
cd "$SOLUTION_DIR"
cmake . \
  -G Xcode \
  -D CMAKE_SYSTEM_NAME=iOS \
  -D "CMAKE_OSX_ARCHITECTURES=arm64;x86_64" \
  -D CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
  -D CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE=YES \
  -B build

xcodebuild -sdk iphonesimulator \
  -arch "x86_64" \
  -project build/webrtc.xcodeproj \
  -target WebRTCPlugin \
  -configuration Release

mv "$WEBRTC_FRAMEWORK_DIR/webrtc.framework" "$WEBRTC_FRAMEWORK_DIR/webrtc-sim.framework"

xcodebuild -sdk iphoneos \
  -project build/webrtc.xcodeproj \
  -target WebRTCPlugin \
  -configuration Release

cd "$WEBRTC_FRAMEWORK_DIR"
lipo -create -o webrtc.framework/webrtc \
  webrtc.framework/webrtc \
  webrtc-sim.framework/webrtc