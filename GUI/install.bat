@assoc .lua=
@ftype lua_gui_application=
@reg delete HKCR\.lua /f

@set CURRENT_FOLDER=%~dp0

@ftype lua_gui_application="%CURRENT_FOLDER%gui.exe" "%%1"
@assoc .lua=lua_gui_application