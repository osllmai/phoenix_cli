#!/bin/bash

DISK_IMAGE="$1"
APP_NAME="PhoenixInstaller"

# Check if PROJECT_SOURCE_DIR is set, otherwise set it manually
if [ -z "$PROJECT_SOURCE_DIR" ]; then
    PROJECT_SOURCE_DIR="/Users/amir/Workspace/clion/phoenix-cli"
fi

# Run the AppleScript to set up the .DS_Store file
osascript "${PROJECT_SOURCE_DIR}/scripts/create_ds_store.applescript.in" "$DISK_IMAGE" "$APP_NAME"
