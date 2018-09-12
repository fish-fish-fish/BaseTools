cd ${PROJECT_DIR}
rm -rf ./build
mkdir -p ./build
log="${PROJECT_DIR}/build/building.log"
cd ./build
echo "Step: begin" >> ${log}
echo "Step: build for arm64 armv7" >> ${log}
xcodebuild -project ${PROJECT_DIR}/BaseTools.xcodeproj -target BaseTools -sdk iphoneos -configuration Release clean build >> ${log}
echo "Step: build for x86_64 i386"
xcodebuild -project ${PROJECT_DIR}/BaseTools.xcodeproj -target BaseTools -sdk iphonesimulator -configuration Release clean build >> ${log}
cp -r ./Release-iphoneos/BaseTools.framework ./
rm ./BaseTools.framework/BaseTools
echo "Step: make fat library." >> ${log}
libtool -static ./Release-iphoneos/BaseTools.framework/BaseTools ./Release-iphonesimulator/BaseTools.framework/BaseTools -o ./BaseTools.framework/BaseTools
lipo -info ./BaseTools.framework/BaseTools >> ${log}
echo "Step: compress files." >> ${log}
zip -r ./BaseTools.framework.zip ./BaseTools.framework
echo "Step: done." >> ${log}
