#!/usr/bin/env bash

APPIMAGETOOL="${APPIMAGETOOL:-appimagetool-x86_64.AppImage}"
LINUXDEPLOY="${LINUXDEPLOY:-linuxdeploy-x86_64.AppImage}"
BUILDDIR="${BUILDDIR:-build}"

echo "Using appimagetool: $APPIMAGETOOL, build dir: $BUILDDIR"

APPDIR="$BUILDDIR/AppDir"
OUTPUT="$BUILDDIR/Marmalade.AppImage"

if [ ! -d "$BUILDDIR/publish" ]; then
  echo "publish directory does not exist. Run marmalade_publish first."
  exit 1
fi


rm -rf "$APPDIR"
mkdir -p "$APPDIR"/usr/bin
mkdir -p "${APPDIR}/usr/share/applications"

cp "$BUILDDIR"/publish/marmalade "$APPDIR"/usr/bin
cp -r "$BUILDDIR"/publish/res "$APPDIR"/res
cp -r res/icons/logo/logo_dark256.png "$APPDIR"/marmalade.png

# Desktop entry
cat > "${APPDIR}/usr/share/applications/marmalade.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=Marmalade
Exec=marmalade
Icon=marmalade
Categories=Game;
EOF

# AppRun launcher
cat > "${APPDIR}/AppRun" <<'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "$0")")"
exec "$HERE/usr/bin/marmalade" "$@"
EOF

chmod +x "${APPDIR}/AppRun"

"$LINUXDEPLOY" --appdir "$APPDIR"

"$APPIMAGETOOL" "$APPDIR" "$OUTPUT" --no-appstream
