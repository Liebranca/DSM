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
version     = "0.1 BETA";

#   ---     ---     ---     ---     ---

from bpy.utils import register_class, unregister_class, register_module, unregister_module
from . import DA_ui;

def register():
    register_module(__name__);
    DA_ui.register();

def unregister():
    DA_ui.unregister();
    unregister_module(__name__);
