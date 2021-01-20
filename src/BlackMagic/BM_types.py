from bpy.utils import register_class, unregister_class
from bpy.types import Scene, Object, Material, PropertyGroup
from bpy.props import StringProperty, EnumProperty, FloatProperty, IntProperty, PointerProperty, BoolProperty

#   ---     ---     ---     ---     ---
# utils I had to write because your API sucks way too hard at convenience.

def BM_setUshort(self, value):
    self = max(0, min(value, 65536));

#   ---     ---     ---     ---     ---

BM_Classes =    [
                    ("static", "Static", "Terrain, building or background prop"),
                    ("actor", "Actor", "Player or NPC"),
                    ("arma", "Armature", "Custom armature logic"),
                ];

BM_ObjectAttrs = [
                    ["meshid", IntProperty,
                    {"name":"Mesh Nº", "default":0,
                    "description":"Object's position in the mesh archive."
                    + " May not be higher than 255 or the archive's element count"}],

                    ["kls", EnumProperty,
                    {"items":BM_Classes, "name":"Type",
                    "description":"Class mutation to be applied ingame"}],

                    ["displayName", StringProperty,
                    {"name":"Name", "default":"Object",
                    "description":"Non-unique name to be displayed ingame. Can be left blank"}],
                    
                    ["weight", FloatProperty,
                    {"name":"Weight", "default":1.0,
                    "description":"Multiplier used by custom physics"}]
                 ];

static_attrs = ["displayName", "weight"];

#   ---     ---     ---     ---     ---

class BM_ObjectSettings(PropertyGroup):

    arc_offset   = IntProperty  (name        = "",                 default = 0, min = 0, max = 255,
                                 description = "DAF file mesh slot"                                              );

#   ---     ---     ---     ---     ---

class BM_SceneSettings(PropertyGroup):

    resourceID   = IntProperty    (name        = "ResourceID",  default = 0, min = 0, max = 65535,
                                   description = "ID of the resource this object belongs to;"
                                              + " used as index into a BM_Resource array"               );

    mesharch     = StringProperty (name        = "MeshArchive", default = "M000",
                                   description = "Name of DAF mesh file to write to"                    );

    texarch      = StringProperty (name        = "MateArchive", default = "T000",
                                   description = "Name of DAF material file to write to"                );

    curmat       = PointerProperty(name         = "",           type    = Material,
                                   description  = "Selected material for adjusting settings & exporting");

    curobj       = PointerProperty(name         = "",           type    = Object,
                                   description  = "Selected object for adjusting settings & exporting"  );

#   ---     ---     ---     ---     ---

BM_MaterialPresets = [
                        ("default",    "Default",    "Specular, opaque, no faux reflections"            ),
                        ("glass",      "Glass",      "Specular, transparency, faux reflections"         ),
                        ("glow",       "Glow",       "No specular, transparency, radiance"              ),

                        ("metalness",  "Metalness",  "Specular, opaque, faux reflections"               ),
                        ("metalglass", "MetalGlass", "Uses glowmap to combine glass and metalness"      ),
                        ("metalglow",  "MetalGlow",  "Uses glowmap to combine glow and metalness"       ),

                        ("sprite",    "Sprite",      "Cam-aligned plane, has animations"                ),
                        ("billboard", "Billboard",   "Cam-aligned plane, no animations"                 ),
                     ];

BM_MaterialFlags   = { 
                       "Specular"   : 0x01,
                       "Opaque"     : 0x02,
                       "Reflective" : 0x04,
                       "Metallic"   : 0x08,
                       "Radiance"   : 0x10,
                       "Animated"   : 0x20,
                       "Sprite"     : 0x40,
                       "NonMat"     : 0x80
                     };

def BM_Material_onPresetChange(self, context):

    scene  = context.scene;
    mate   = scene.BlackMagic.curmat;

    newkls = mate.BlackMagic.preset;

    if newkls in ["default", "glass", "metalness", "metalglass", "metalglow", "billboard", "sprite"]:

        mate.BlackMagic.num_textures = 3;

        if   newkls == "default"   :
            mate.BlackMagic.shader = 0;
            mate.BlackMagic.flags  = (BM_MaterialFlags["Specular"  ]
                                   |  BM_MaterialFlags["Opaque"    ]);

        elif newkls == "glass"     :
            mate.BlackMagic.shader = 1;
            mate.BlackMagic.flags  = (BM_MaterialFlags["Specular"  ]
                                   |  BM_MaterialFlags["Reflective"]);

        elif newkls == "metalness" :
            mate.BlackMagic.shader = 2;
            mate.BlackMagic.flags  = (BM_MaterialFlags["Specular"  ]
                                   |  BM_MaterialFlags["Opaque"    ]
                                   |  BM_MaterialFlags["Metallic"  ]
                                   |  BM_MaterialFlags["Reflective"]);

        elif newkls == "metalglass":
            mate.BlackMagic.shader = 3;
            mate.BlackMagic.flags  = (BM_MaterialFlags["Specular"  ]
                                   |  BM_MaterialFlags["Metallic"  ]
                                   |  BM_MaterialFlags["Reflective"]);

        elif newkls == "metalglow" :
            mate.BlackMagic.shader = 4;
            mate.BlackMagic.flags  = (BM_MaterialFlags["Specular"  ]
                                   |  BM_MaterialFlags["Metallic"  ]
                                   |  BM_MaterialFlags["Reflective"]
                                   |  BM_MaterialFlags["Radiance"  ]);

        elif newkls == "billboard" :
            mate.BlackMagic.shader = 5;
            mate.BlackMagic.flags  = (BM_MaterialFlags["Specular"  ]
                                   |  BM_MaterialFlags["Opaque"    ]
                                   |  BM_MaterialFlags["Sprite"    ]);

        elif newkls == "sprite"    :
            mate.BlackMagic.shader = 6;
            mate.BlackMagic.flags  = (BM_MaterialFlags["Specular"  ]
                                   |  BM_MaterialFlags["Opaque"    ]
                                   |  BM_MaterialFlags["Sprite"    ]
                                   |  BM_MaterialFlags["Animated"  ]);

    elif newkls == "glow":

        mate.BlackMagic.shader       = 7;
        mate.BlackMagic.num_textures = 2;
        mate.BlackMagic.flags        = BM_MaterialFlags["Radiance"];

class BM_MaterialSettings(PropertyGroup):

    preset       = EnumProperty (items       = BM_MaterialPresets, name    = "", default = "default",
                                 description = "Determines SIN shader for this material & some export settings",
                                 update      = BM_Material_onPresetChange                                        );

    num_textures = IntProperty  (name        = "",                 default = 3, min = 0, max = 255,
                                 description = "Number of texture maps to export"                                );

    tex_offset   = IntProperty  (name        = "",                 default = 0, min = 0, max = 255,
                                 description = "DAF file texture slot"                                           );

    mat_offset   = IntProperty  (name        = "",                 default = 0, min = 0, max = 255,
                                 description = "DAF file material slot"                                          );

    matid        = IntProperty  (name        = "",                 default = 1, min = 1, max = 65535,
                                 description = "Unique material ID (nonzero)"                                    );

    flags        = IntProperty  (default     = BM_MaterialFlags["Specular"] | BM_MaterialFlags["Opaque"]         );

    shader       = IntProperty  (default     = 0                                                                 );

    spec_mult    = FloatProperty(name        = "",                 default = 0.65, min = 0, max = 1.49,
                                 description = "Intensity factor for specular"                                   );

    diff_mult    = FloatProperty(name        = "",                 default = 0.50, min = 0, max = 1.49,
                                 description = "Intensity factor for diffuse"                                    );

    ref_mult     = FloatProperty(name        = "",                 default = 0.75, min = 0, max = 1.49,
                                 description = "Intensity factor for faux-reflections"                           );

    glow_rea     = FloatProperty(name        = "",                  default = 2.00, min = 0, max = 8.00,
                                 description = "Effective distance for the radiance effect"                      );

#   ---     ---     ---     ---     ---

def register():

    Scene.BlackMagic    = PointerProperty(type = BM_SceneSettings   );
    Object.BlackMagic   = PointerProperty(type = BM_ObjectSettings  );
    Material.BlackMagic = PointerProperty(type = BM_MaterialSettings);

def unregister():
    
    del Scene.BlackMagic;
    del Object.BlackMagic;
    del Material.BlackMagic;

#   ---     ---     ---     ---     ---
