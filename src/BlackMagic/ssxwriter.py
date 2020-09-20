from .bmutils import *

obj_proplist  = ["resid", "obarci"]

#   ---     ---     ---     ---     ---

def writessx(pot_name):

    from . import filepath

    scene_objects = [ob for ob in bpy.context.scene.objects if ob.layers[0]];
    ssx           = DA_Scene(len(scene_objects));

#   ---     ---     ---     ---     ---

    for ob in scene_objects:

        for propname in obj_proplist:
            if propname not in ob.game.properties: raise STAHP ("Object %s missing property %s"%(ob.name, propname));

        buff = make_DA_Object(ob.game.properties["resid"].value,
                              ob.game.properties["obarci"].value,
                              ob.location,
                              ob.rotation_quaternion,
                              ob.scale                             )

        ssx.add_entry(buff);

#   ---     ---     ---     ---     ---

    with open(filepath + "\\" + pot_name + ".ssx", "wb+") as file: file.write(ssx.buff);

#   ---     ---     ---     ---     ---
