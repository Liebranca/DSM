from bpy.types import UIList

from DarkAge.bl.types.prop import read_strList, read_strDict;
from DarkAge.bl.ops import prop as prop_ops;

import DarkAge as DA

#   ---     ---     ---     ---     ---

class GamePropList(UIList):

    def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):

        if item:

            ic   = "RADIOBUT_OFF";
            ob   = DA.gbptr;
            prop = ob.props[ob.curprop];

            if item == prop:
                ic = "RADIOBUT_ON";

            if self.layout_type in {'DEFAULT', 'COMPACT'}:

                value = item.value;

                if item.kls   == 'LIST': value = str(read_strList(item.value));
                elif item.kls == 'DICT': value = str(read_strDict(item.value));

                layout.prop(item, "name", icon=ic, emboss=False, text=""); layout.label(value);

            elif self.layout_type == 'GRID':
                layout.alignment = 'CENTER';
                layout.label(text="", icon=ic);

#   ---     ---     ---     ---     ---

def drawPropBlock(layout):

    ob  = DA.gbptr;

    layout.separator();
    row = layout.row(); row.label("Game Properties");

    row = layout.row();
    row.template_list("GamePropList", "", ob, "props", ob, "curprop", rows=1, maxrows=4, type='DEFAULT')

    col = row.column(True);

    col.operator("darkage.addgameprop",  text = "", icon = "ZOOMIN"   );
    col.operator("darkage.delgameprop",  text = "", icon = "ZOOMOUT"  );
    col.operator("darkage.editgameprop", text = "", icon = "SCRIPTWIN");

#   ---     ---     ---     ---     ---
