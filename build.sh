#!/usr/bin/env bash
set -e

BUILD_DIR="build"
TARGET="app"

configure() {
    cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Debug}
}

build() {
    cmake --build "$BUILD_DIR" --parallel
}

run() {
    cd "$BUILD_DIR" && ./app
}

echo "What do you want to do?"
echo "1) Build & Run"
echo "2) Rebuild (clean)"
echo "3) Build only"
echo "4) Run only"
echo "5) Release build"
echo "6) Clean"
read -rp "> " choice

case "$choice" in
    1) configure && build && run ;;
    2) rm -rf "$BUILD_DIR" && configure && build && run ;;
    3) configure && build ;;
    4) run ;;
    5) BUILD_TYPE=Release configure && build ;;
    6) rm -rf "$BUILD_DIR" && echo "Cleaned." ;;
    *) echo "Invalid choice." && exit 1 ;;
esac
