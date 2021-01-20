from .BM_utils import *

obj_proplist  = ["resid", "obarci", "flags"]
lamp_proplist = ["lradius", "latten"]

#   ---     ---     ---     ---     ---

def writessx(ssxname):

    from . import filepath

    scene_objects = [ob for ob in bpy.context.scene.objects if ob.layers[0]];
    ssx           = DA_Scene();

#   ---     ---     ---     ---     ---

    for ob in scene_objects:

        if ob.data:

          if isinstance(ob.data, bpy.types.Mesh) and not ob.parent:
            ob.rotation_mode = "QUATERNION";

            for propname in obj_proplist:
                if propname not in ob.game.properties: raise STAHP ("Object %s missing property %s"%(ob.name, propname));

#   ---     ---     ---     ---     ---

            lamp = None;
            for child in ob.children:

                if (isinstance(child.data, bpy.types.PointLamp)
                or  isinstance(child.data, bpy.types.SunLamp  )
                or  isinstance(child.data, bpy.types.SpotLamp )
                or  isinstance(child.data, bpy.types.HemiLamp )
                or  isinstance(child.data, bpy.types.AreaLamp )):
                    lamp = child; break;

#   ---     ---     ---     ---     ---

            if lamp:

                for propname in lamp_proplist:
                    if propname not in ob.game.properties: raise STAHP ("Object %s missing property %s"%(ob.name, propname));

                colarr = [lamp.color[0], lamp.color[1], lamp.color[2], lamp.energy];

                size = 1;
                buff = make_DA_Lamp  (ob.game.properties["resid"].value,
                                      ob.game.properties["obarci"].value,
                                      ob.game.properties["flags"].value,
                                      ob.location,
                                      ob.rotation_quaternion,
                                      ob.scale,

                                      lamp.location,
                                      colarr,
                                      [0,-1,0],

                                      ob.game.properties["rad"].value,
                                      ob.game.properties["atten"].value,   )

#   ---     ---     ---     ---     ---

            else:

                size = 0;
                buff = make_DA_Object(ob.game.properties["resid"].value,
                                  ob.game.properties["obarci"].value,
                                  ob.game.properties["flags"].value,
                                  ob.location,
                                  ob.rotation_quaternion,
                                  ob.scale                             )

            ssx.add_entry(buff, size);
            ob.rotation_mode = "XYZ";

#   ---     ---     ---     ---     ---

    ssx.write(filepath, ssxname);

#   ---     ---     ---     ---     ---
