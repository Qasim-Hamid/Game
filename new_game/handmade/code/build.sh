mkdir ../../build &>/dev/null
pushd ../../build &>/dev/null
cl.exe -FC -Zi ../handmade/code/win32_handmade.cpp user32.lib Gdi32.lib
popd &>/dev/null
