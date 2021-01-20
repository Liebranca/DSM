import bpy, sys

from bpy.utils import register_class, unregister_class
from bpy.types import UIList

from . import DA_ops, DA_types

#   ---     ---     ---     ---     ---

class TriggerList(UIList):

    def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):

        if item:

            ic    = "RADIOBUT_OFF"
            ob    = context.object.DarkAge;
            state = ob.states[int(ob.curstate)]

            if item == state.triggers[state.active_trigger]:
                ic = "RADIOBUT_ON"

        if self.layout_type in {'DEFAULT', 'COMPACT'}:
            layout.prop(item, "name", text="", emboss=False, icon=ic);

        elif self.layout_type == 'GRID':
            layout.alignment = 'CENTER';
            layout.label(text="", icon=ic);

#   ---     ---     ---     ---     ---

class SB_Panel(bpy.types.Panel):

    bl_label       = 'DarkAge';
    bl_idname      = 'SB_panel';
    bl_space_type  = 'PROPERTIES';
    bl_region_type = 'WINDOW';
    bl_context     = "object";

#   ---     ---     ---     ---     ---
    
    @classmethod
    def poll(cls, context):
        return context.object is not None;

    def draw(self, context):

        layout   = self.layout;

        scene    = context.scene;
        ob       = context.object.DarkAge;

        if not len(ob.states):
            row  = layout.row(True);
            row.operator("darkage.objectinit", text = "Initialize", icon = "NEW");

#   ---     ---     ---     ---     ---

        else:

            curstate      = int            (ob.curstate);
            state         = ob.states      [curstate   ];

            row           = layout.row(); row.label("Current: %s"%state.name);
            row           = layout.row();

            col           = row.column_flow(17, True);

            col.scale_x   = 0.75;
            col.scale_y   = 0.75;

#   ---     ---     ---     ---     ---

            for i in range(15):

                if i and not (i%5):
                    col.label(""); col.label("");

                for j in [i, i+15]:

                    ic = 'NONE';

                    if ob.states[j].used:
                        ic = 'LAYER_USED';
                        if int(curstate) == j: ic = 'LAYER_ACTIVE';

                    col.prop_enum(ob, "curstate", "%i"%j, text = "", icon = ic);

#   ---     ---     ---     ---     ---

            layout.separator();

            row = layout.row();

            row.prop(state, "used"); row.label("ON" if state.used else "OFF");
            row.prop(state, "name");

#   ---     ---     ---     ---     ---

            layout.separator();

            row = layout.row();
            row.template_list("TriggerList", "", state, "triggers", state, "active_trigger", rows=1, maxrows=4, type='DEFAULT')

            col = row.column(True);

            col.operator("darkage.addtrigger",  text = "", icon = "ZOOMIN"   );
            col.operator("darkage.deltrigger",  text = "", icon = "ZOOMOUT"  );
            col.operator("darkage.edittrigger", text = "", icon = "SCRIPTWIN");

#   ---     ---     ---     ---     ---

def register():
    DA_ops.register();
    DA_types.register();

def unregister():
    DA_ops.unregister();
    DA_types.unregister();
