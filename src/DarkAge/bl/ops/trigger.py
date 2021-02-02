from bpy.types import Operator
from bpy.props import BoolProperty, IntProperty, EnumProperty, StringProperty

from DarkAge.bl.utils.lystr import ensureNameUnique;

from DarkAge.bl.types.prop import PropTypes, read_strList;
from DarkAge.bl.types.prop import read_strDict, morphGamePropValue;

import DarkAge as DA;

class DA_addTrigger(Operator):

    bl_idname      = "darkage.addtrigger";
    bl_label       = "Add a new trigger";

    bl_description = "Adds a new trigger to the current state";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob               = DA.gbptr;
        state            = ob.states[int(ob.curstate)];
        new_trigger      = state.triggers.add();
        new_trigger.idex = len(state.triggers) - 1;

        new_trigger.name = ensureNameUnique(state.triggers, new_trigger.idex);

        return {'FINISHED'}

class DA_delTrigger(Operator):

    bl_idname      = "darkage.deltrigger";
    bl_label       = "Removes a trigger";

    bl_description = "Removes active trigger from current state";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob          = DA.gbptr;
        state       = ob.states[int(ob.curstate)];

        if len(state.triggers): state.triggers.remove(state.active_trigger);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class SB_editTrigger(Operator):

    bl_idname      = "darkage.edittrigger";
    bl_label       = "Edit selected trigger";

    bl_description = "Brings up the trigger editing pop-up";

#   ---     ---     ---     ---     ---

    def draw(self, context):

        ob           = DA.gbptr;
        state        = ob.states[int(ob.curstate)];
        trigger      = state.triggers[state.active_trigger];

        layout       = self.layout;
        row          = layout.row();

        row.prop(trigger, "name"); row.prop(trigger, "kls");

#   ---     ---     ---     ---     ---

        layout.separator();

        if trigger.kls == "mess":
            pass;

#   ---     ---     ---     ---     ---

    def execute(self, context): return {'FINISHED'};

    def invoke(self, context, event):

        ob           = DA.gbptr;
        state        = ob.states[int(ob.curstate)];

        if not len(state.triggers): return {'CANCELLED'};

        wm = context.window_manager;
        return wm.invoke_props_dialog(self, width=600);

#   ---     ---     ---     ---     ---

def register(): pass;   

def unregister(): pass;