mkdir ../build >/dev/null 2>&1
pushd ../build >/dev/null 2>&1
cl.exe -FC -Zi ../code/win32_handmade.cpp user32.lib Gdi32.lib
popd >/dev/null 2>&1
