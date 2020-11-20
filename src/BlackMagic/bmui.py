import bpy, sys

from bpy.utils import register_class, unregister_class

from . import bmtypes
from . import bmpopup
from . import bmcommops

#   ---     ---     ---     ---     ---
    
class DA_scenePanel(bpy.types.Panel):

    bl_label       = 'Global';
    bl_idname      = 'DA_scenePanel';
    bl_space_type  = 'VIEW_3D';
    bl_region_type = 'TOOLS';
    bl_category    = 'BlackMagic';

#   ---     ---     ---     ---     ---
    
    @classmethod
    def poll(cls, context):
        return context.scene is not None;

    def draw(self, context):
        layout = self.layout;

        scene  = context.scene;

        row = layout.row();
        row.prop(scene.BlackMagic, "resourceID");

        layout.separator();

        row = layout.row();
        row.prop(scene.BlackMagic, "mesharch");
        row = layout.row();
        row.prop(scene.BlackMagic, "texarch");

        layout.separator();

#   ---     ---     ---     ---     ---

class DA_objectPanel(bpy.types.Panel):

    bl_label       = 'Object';
    bl_idname      = 'DA_objectPanel';
    bl_space_type  = 'VIEW_3D';
    bl_region_type = 'TOOLS';
    bl_category    = 'BlackMagic';

#   ---     ---     ---     ---     ---
    
    @classmethod
    def poll(cls, context):
        return context.scene is not None;

    def draw(self, context):
        layout = self.layout;

        scene  = context.scene;
        ob     = scene.BlackMagic.curobj;

        row = layout.row();
        layout.prop_search(scene.BlackMagic, "curobj", bpy.data, "objects");

        if ob:

            row = layout.row();
            row.label(text = "Slot:")
            row.prop(ob.BlackMagic, "arc_offset");

            layout.separator();

            row = layout.row();
            row.operator("blackmagic.exportmesh", text = "Export", icon = "EXPORT");

            layout.separator();

#   ---     ---     ---     ---     ---

class DA_materialPanel(bpy.types.Panel):

    bl_label       = 'Material';
    bl_idname      = 'DA_materialPanel';
    bl_space_type  = 'VIEW_3D';
    bl_region_type = 'TOOLS';
    bl_category    = 'BlackMagic';

#   ---     ---     ---     ---     ---
    
    @classmethod
    def poll(cls, context):
        return context.scene is not None;

    def draw(self, context):
        layout = self.layout;

        scene  = context.scene;

        row = layout.row();
        layout.prop_search(scene.BlackMagic, "curmat", bpy.data, "materials");

        mate = scene.BlackMagic.curmat;

        if mate:

            layout.separator();

            row = layout.row();
            row.prop(mate.BlackMagic, "preset");
            row.operator("blackmagic.setmateflags", text = "Set Flags", icon = "MOD_SOFT");

            layout.separator();

            row = layout.row();
            row.label(text = "ID:");
            row.prop(mate.BlackMagic, "matid");

            row = layout.row();
            row.label(text = "TexCount:");
            row.prop(mate.BlackMagic, "num_textures");

            row = layout.row();
            row.label(text = "TexSlot:");
            row.prop(mate.BlackMagic, "tex_offset");

            row = layout.row();
            row.label(text = "MatSlot:");
            row.prop(mate.BlackMagic, "mat_offset");

            layout.separator();

            if mate.BlackMagic.flags & bmtypes.DA_MaterialFlags["Specular"]:
                row = layout.row();
                row.label(text = "Specular:");
                row.prop(mate.BlackMagic, "spec_mult");

            row = layout.row();
            row.label(text = "Diffuse:");
            row.prop(mate.BlackMagic, "diff_mult");

            if mate.BlackMagic.flags & bmtypes.DA_MaterialFlags["Reflective"]:
                row = layout.row();
                row.label(text = "Reflection:");
                row.prop(mate.BlackMagic, "ref_mult");

            if mate.BlackMagic.flags & bmtypes.DA_MaterialFlags["Radiance"]:
                row = layout.row();
                row.label(text = "Radiance:");
                row.prop(mate.BlackMagic, "glow_rea");

            layout.separator();
            
            row = layout.row();
            row.operator("blackmagic.exportmaterial", text = "Export", icon = "EXPORT");

        layout.separator();

#   ---     ---     ---     ---     ---

def register():
    bmtypes.register  ();
    bmpopup.register  ();
    bmcommops.register();

    register_class(DA_scenePanel   );
    register_class(DA_objectPanel  );
    register_class(DA_materialPanel);


def unregister():
    bmtypes.unregister  ();
    bmpopup.unregister  ();
    bmcommops.unregister();

    unregister_class(DA_scenePanel   );
    unregister_class(DA_objectPanel  );
    unregister_class(DA_materialPanel);

if __name__ == "__main__":
    register();

#   ---     ---     ---     ---     ---
