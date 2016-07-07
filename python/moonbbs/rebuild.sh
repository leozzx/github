#!/bin/bash

echo "Removing old files"
rm -rf ./dist/
rm -rf /Applications/moonbbs.app

echo "Build..."
pyinstaller moonbbs.spec

cp icon-windowed.icns ./dist/moonbbs.app/Contents/Resources/icon-windowed.icns

echo "New files"
cp -r ./dist/moonbbs.app /Applications/

rm -rf ./dist/
