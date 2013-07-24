rmdir /s /q "xml"
mkdir xml
rmdir /s /q "Lua"
mkdir Lua

doxygen GameplayBridge.doxyfile
..\..\..\GamePlay\bin\windows\gameplay-luagen.exe ./xml/ ./lua/ gameplay