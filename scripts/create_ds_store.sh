tell application "Finder"
    tell disk "@CPACK_PACKAGE_NAME@ Installer"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {100, 100, 512, 384}
        set theViewOptions to the icon view options of container window
        set arrangement of theViewOptions to not arranged
        set icon size of theViewOptions to 128
        set background picture of theViewOptions to file ".background:background.png"
        set position of item "@CPACK_PACKAGE_NAME@.app" of container window to {128, 192}
        make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
        set position of item "Applications" of container window to {384, 192}
        close
        open
        update without registering applications
    end tell
end tell