mkdir ../../build &>/dev/null
pushd ../../build &>/dev/null
cl.exe -Zi ../handmade/code/win32_handmade.cpp user32.lib
popd &>/dev/null
