#!/bin/bash
set -euo pipefail

# M.U.G.E.N Engine Launcher
# This script runs the Go implementation of the MUGEN engine or its specific test suites.

# Get the directory where this script is located
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Keep the root module read-only and all disposable build/test files in scope.
mkdir -p "$DIR/temp/go-build" "$DIR/temp/tmp"
export GOCACHE="$DIR/temp/go-build"
export GOTMPDIR="$DIR/temp/tmp"
export TMPDIR="$DIR/temp/tmp"
export GOFLAGS="${GOFLAGS:+$GOFLAGS }-modfile=$DIR/mugen.mod -mod=readonly"
cd "$DIR/../.."

show_help() {
    echo "Usage: $0 [mode] [parameters]"
    echo ""
    echo "Modes:"
    echo "  --game          Run the main game (default if no mode specified)"
    echo "  --test <name>   Run a specific test suite or unit test"
    echo "  --check         Build and test every MUGEN package"
    echo "  --reference-tools Build the isolated Windows diagnostic helper"
    echo "  --help          Show this help message"
    echo ""
    echo "Available test applications (go run):"
    for test_dir in "$DIR"/tests/*/; do
        if [ -f "${test_dir}main.go" ]; then
            tname=$(basename "${test_dir}")
            echo "  - $tname"
        fi
    done
    echo ""
    echo "Available unit tests (go test):"
    for test_file in "$DIR"/tests/*_test.go; do
        if [ -f "$test_file" ]; then
            tname=$(basename "$test_file" _test.go)
            echo "  - $tname"
        fi
    done
    echo ""
    echo "Example:"
    echo "  $0 --game --data-dir game/mugen/temp/mugen-1.1b1"
    echo "  $0 --test character --data-dir game/mugen/temp/mugen-1.1b1"
    echo "  $0 --check"
    echo "  $0 --test parser"
}

# Default mode
MODE="game"
TARGET=""

# Argument parsing
if [[ $# -eq 0 ]]; then
    MODE="game"
elif [[ "$1" == "--help" ]]; then
    show_help
    exit 0
elif [[ "$1" == "--game" ]]; then
    MODE="game"
    shift
elif [[ "$1" == "--test" ]]; then
    MODE="test"
    TARGET="${2:-}"
    if [[ -z "$TARGET" ]]; then
        echo "Error: --test requires a test name."
        show_help
        exit 1
    fi
    shift 2
elif [[ "$1" == "--check" ]]; then
    shift
    exec go test ./game/mugen/... "$@"
elif [[ "$1" == "--reference-tools" ]]; then
    mkdir -p "$DIR/temp/reference-tools"
    exec env GOOS=windows GOARCH=386 CGO_ENABLED=0 go build -o "$DIR/temp/reference-tools/wintext.exe" "$DIR/tests/reference_text"
elif [[ "$1" == --* ]]; then
    # If the first arg is a flag (like --data-dir), it implies game mode
    MODE="game"
fi

# Execution
if [[ "$MODE" == "test" ]]; then
    # Check if it's an application test (directory with main.go)
    APP_PATH="$DIR/tests/$TARGET/main.go"
    # Check if it's a unit test file
    UNIT_PATH="$DIR/tests/${TARGET}_test.go"
    
    if [[ -f "$APP_PATH" ]]; then
        go run "$DIR/tests/$TARGET" "$@"
    elif [[ -f "$UNIT_PATH" ]]; then
        # For unit tests, we run the entire tests package but can filter if needed
        # Forwarding additional args allows the user to pass -run etc.
        go test -v "$DIR/tests" "$@"
    else
        echo "Error: Test target '$TARGET' not found."
        show_help
        exit 1
    fi
else
    go run "$DIR/system" "$@"
fi
