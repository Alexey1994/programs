@assoc .lua=
@ftype lua_application=

@set CURRENT_FOLDER=%~dp0

@ftype lua_application="%CURRENT_FOLDER%lua.exe" "%%1"
@assoc .lua=lua_application