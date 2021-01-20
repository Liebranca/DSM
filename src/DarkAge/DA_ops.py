import bpy

from bpy.types import Operator
from bpy.props import BoolProperty, EnumProperty, StringProperty
from bpy.utils import register_class, unregister_class

from .DA_utils import ensureNameUnique;

#   ---     ---     ---     ---     ---

class DA_objectInit(Operator):

    bl_idname      = "darkage.objectinit";
    bl_label       = "Initializer for DarkAge";

    bl_description = "An object initializer necessary to bypass the utter ineptitude of this API";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob = context.object.DarkAge;

        for i in range(30):
            new_state = ob.states.add();
            new_state.name = "State %i"%i;

        self.report({'INFO'}, "Object was initialized");
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_addTrigger(Operator):

    bl_idname      = "darkage.addtrigger";
    bl_label       = "Add a new trigger";

    bl_description = "Adds a new trigger to the current state";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob               = context.object.DarkAge;
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

        ob          = context.object.DarkAge;
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

        ob           = context.object.DarkAge;
        state        = ob.states[int(ob.curstate)];
        trigger      = state.triggers[state.active_trigger];

        layout       = self.layout;
        row          = layout.row();

        row.prop(trigger, "name"); row.prop(trigger, "kls");

    def execute(self, context): return {'FINISHED'};

    def invoke(self, context, event):

        ob           = context.object.DarkAge;
        state        = ob.states[int(ob.curstate)];

        if not len(state.triggers): return {'CANCELLED'};

        wm = context.window_manager;
        return wm.invoke_props_dialog(self, width=600);

#   ---     ---     ---     ---     ---

def register(): pass;   

def unregister(): pass;