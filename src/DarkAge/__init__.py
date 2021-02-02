bl_info = {
    "name": "DarkAge",
    "author": "Liebranca",
    "version": (1, 0),
    "blender": (2, 79, 7),
    "location": "Properties > Object > DarkAge",
    "description": "Game logic & animation manager",
    "warning": "",
    "wiki_url": "",
    "category": "Game Engine",
}

#   ---     ---     ---     ---     ---

darkpath    = "D:\\lieb_git\\dsm\\src\\DarkAge";
version     = "0.1a";

#   ---     ---     ---     ---     ---

gbptr = None; _globals = None; conts = []; inputMorph = {};

def register():

    from bpy.utils import register_module

    from .bl import utils, types, ui, ops;
    from .bl.types import gblock as GameLogic;

    register_module(__name__);
    GameLogic.register();

    ui.panels.GlobalsPanel_Cont.addBox(_globals.props);
    conts.append(ui.panels.GlobalsPanel_Cont);

    global inputMorph;
    
    from .bl.types.prop import propStr_toInt, propStr_toFloat, propStr_toBool, morphGamePropValue;
    inputMorph = { 'INT':propStr_toInt, 'FLOAT':propStr_toFloat, 'BOOL':propStr_toBool, 'PROP':morphGamePropValue };

def unregister():

    from bpy.utils import unregister_module
    from .bl.types import gblock as GameLogic;

    GameLogic.unregister();
    unregister_module(__name__);

#   ---     ---     ---     ---     ---
