# EnumNameMaker
Generate a script containing enum name bindings for Gamemaker.

-----------

It can be used 2 ways.
1. Search all folder recursively directly in the project
2. Drag and drop all selected .gml files on the exe

When done the 

-----------
USING METHOD #1
-----------
1. Create a folder called external_tools at the root of your project.
    animcurves
    datafiles
    external_tools      <---- 
    notes
    objects
    etc....
   
2. Place EnumNameMaker.exe inside the external_tools.

3. Open your project and create a folder "Scripts" at the base (not inside other folders).

4. Create a script resource called enum_bindings.gml inside that Scripts Folder.

5. Save your project. Don't manually edit this script as the exe overrides it.

6. Run the EnumNameMaker.exe

7. Wait until completion and check the enum_bindings.gml for the generated code.

-----------
USING METHOD #2
-----------
1. Just drag and drop all .gml files to process onto the EnumNameMaker.exe
2. A enum_bindings.gml file will be created in the same file location

-----------

enum KEY
{
  LEFT = 55,
  RIGHT = #2356
};

var _enum_name = enum_binding_get_name( "KEY", KEY.LEFT );
show_debug_message( _enum_name ); // prints "KEY.LEFT"

var _enum_value = enum_binding_get_value( "KEY.LEFT" );
show_debug_message( _enum_value ); // prints 55

-----------
