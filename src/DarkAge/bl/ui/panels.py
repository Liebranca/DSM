from bpy.types import Panel, PropertyGroup;
from bpy.props import EnumProperty, StringProperty, PointerProperty;

from .object_properties import drawPropBlock;
from .object_states import drawStateBlock;
from .object_triggers import drawTriggerBlock;

from .controller import DA_UIController;

import DarkAge as DA

#   ---     ---     ---     ---     ---

def getGlobalStates(self, context):
    pass;

#   ---     ---     ---     ---     ---

GlobalsPanel_Cont = DA_UIController(0);

class DA_GlobalsPanel(Panel):

    bl_label       = 'DarkAge';
    bl_idname      = 'DA_Globals';
    bl_space_type  = 'PROPERTIES';
    bl_region_type = 'WINDOW';
    bl_context     = 'render_layer';

#   ---     ---     ---     ---     ---
    
    @classmethod
    def poll(cls, context):
        return context.scene is not None;

    def draw(self, context):

        layout   = self.layout;

        scene    = context.scene;
        ob       = DA._globals;
        cont     = GlobalsPanel_Cont;

        cont.curiter = ob.props;
        cont.boxes[0].draw(context, layout, cont.idex);

#   ---     ---     ---     ---     ---
