from bpy.types import UIList

from DarkAge.bl.ops import trigger as trigger_ops;
import DarkAge as DA

#   ---     ---     ---     ---     ---

class TriggerList(UIList):

    def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):

        if item:

            ic    = "RADIOBUT_OFF"
            ob    = DA.gbptr;
            state = ob.states[int(ob.curstate)]

            if item == state.triggers[state.active_trigger]:
                ic = "RADIOBUT_ON"

            if self.layout_type in {'DEFAULT', 'COMPACT'}:
                layout.prop(item, "name", text="", emboss=False, icon=ic);

            elif self.layout_type == 'GRID':
                layout.alignment = 'CENTER';
                layout.label(text="", icon=ic);

#   ---     ---     ---     ---     ---

def drawTriggerBlock(layout):

    ob       = DA.gbptr;
    
    curstate = int      (ob.curstate);
    state    = ob.states[curstate   ];

    layout.separator();
    row = layout.row(); row.label("Triggers for %s %s"%(state.name, "State" if state.name[0].isupper() else "state"));

    row = layout.row();
    row.template_list("TriggerList", "", state, "triggers", state, "active_trigger", rows=1, maxrows=4, type='DEFAULT')

    col = row.column(True);

    col.operator("darkage.addtrigger",  text = "", icon = "ZOOMIN"   );
    col.operator("darkage.deltrigger",  text = "", icon = "ZOOMOUT"  );
    col.operator("darkage.edittrigger", text = "", icon = "SCRIPTWIN");

#   ---     ---     ---     ---     ---
