import bpy

from bpy.types import Operator
from bpy.props import BoolProperty
from bpy.utils import register_class, unregister_class

from .jojwriter import writejoj

#   ---     ---     ---     ---     ---

class DA_materialExport(Operator):

    bl_idname      = "blackmagic.exportmaterial";
    bl_label       = "Export selected material to DarkAge";

    bl_description = "Exports material to selected material archive";

    valid          = 1;

    def __init__(self):

        self.mate = bpy.context.scene.BlackMagic.curmat;

        if not self.mate:
            self.report({'ERROR'}, "No material selected in BlackMagic panel!");
            self.valid = 0; return;

        super().__init__();

#   ---     ---     ---     ---     ---

    def execute(self, context):

        if not self.valid: return {'CANCELLED'};
        evilstate = writejoj();

        if not evilstate:
            self.report({'INFO'}, "Material %s saved to %s"%(self.mate.name, bpy.context.scene.BlackMagic.texarch));
            return {'FINISHED'}

        else:
            self.report({'ERROR'}, "%s *.JOJ conversion failed."%evilstate);
            return {'CANCELLED'};

#   ---     ---     ---     ---     ---

def register():

    register_class(DA_materialExport);

def unregister():

    unregister_class(DA_materialExport);

#   ---     ---     ---     ---     ---