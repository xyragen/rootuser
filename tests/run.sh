#!/bin/sh
set -e
cd "$(dirname "$0")/.."

# help and version work without any privilege
./build/rous --help | grep -q "usage: rous"
./build/rous -h | grep -q "run a command as the root user"

# no command is an error
if ./build/rous 2>/dev/null; then
  echo "expected failure with no command"
  exit 1
fi

# an unknown option is an error
if ./build/rous --nope id 2>/dev/null; then
  echo "expected failure on unknown option"
  exit 1
fi

# running a command that does not exist reports not found
if ./build/rous definitely-not-a-real-binary-xyz 2>/dev/null; then
  echo "expected failure on missing command"
  exit 1
fi

# when not installed setuid root, rous refuses instead of lying
if [ "$(id -u)" != "0" ]; then
  if ./build/rous id 2>/dev/null; then
    echo "expected refusal when not privileged"
    exit 1
  fi
  ./build/rous id 2>&1 | grep -q "not privileged"
fi

echo "run.sh ok"
