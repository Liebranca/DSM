from bpy.types import Operator
from bpy.props import BoolProperty, IntProperty, EnumProperty, StringProperty

from DarkAge.bl.utils.lystr import ensureNameUnique;

from DarkAge.bl.types.prop import PropTypes, read_strList;
from DarkAge.bl.types.prop import read_strDict, morphGamePropValue;

import DarkAge as DA

#   ---     ---     ---     ---     ---

class DA_objectInit(Operator):

    bl_idname      = "darkage.objectinit";
    bl_label       = "Initializer for DarkAge";

    bl_description = "An object initializer necessary to bypass the utter madness of this API";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob = DA.gbptr;

        for i in range(30):
            new_state = ob.states.add();
            new_state.name = "State %i"%i;

        self.report({'INFO'}, "Object was initialized");
        return {'FINISHED'}

#   ---     ---     ---     ---     ---
