Plugins are built as DLLs and placed into Plugins folder in Screenberry binary folder. For example:
c:\Front Pictures\Screenberry 3.1.0\Screenberry\Plugins\TestPlugin\TestPlugin.dll
and, optionally, widget in the same folder:
Plugins\TestPlugin\TestPluginWidget.dll
(see Example/bin folder)

Server side plugin exports one method to create SBNode instance
Panel side plugin exports one method to create QWidget instance. 
It should use same Qt version as ScreenberryPanel. Currently for SB 3.1.0 it is Qt 6.4.3
