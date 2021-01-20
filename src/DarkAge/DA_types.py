from bpy.types import Scene, Object, Action, Material, PropertyGroup;
from bpy.props import StringProperty, EnumProperty, FloatProperty, IntProperty, PointerProperty;
from bpy.props import BoolProperty, CollectionProperty;

from bpy.utils import register_class;

from .DA_utils import ensureNameUnique;

#   ---     ---     ---     ---     ---

class ActionPointer(PropertyGroup):
    val = PointerProperty(type = Action);

register_class(ActionPointer);

TriggerTypes =  [
                    ("prop",  "Property", "Activates through property evaluation"),
                    ("input", "Input",    "Activates via input"                  ),
                    ("mess",  "Message",  "Activates via messages"               ),
                ];

def uNameWrap_Trigger(self, context):

    ob                = context.object.DarkAge
    state             = ob.states[int(ob.curstate)];

    unique_name       = ensureNameUnique(state.triggers, self.idex);

    if unique_name != self.name:
        self.name = unique_name;    

class LogicTrigger(PropertyGroup):

    name         = StringProperty    (name = "Name", default = "Trigger", update = uNameWrap_Trigger);
    kls          = EnumProperty      (name = "Type", items  = TriggerTypes, default = "prop"        );
    idex         = IntProperty       (                                                              );

register_class(LogicTrigger);

class LogicState(PropertyGroup):

    used           = BoolProperty      (name = "", default = 0            );
    name           = StringProperty    (name = "Name", default = "State"  );
    numAnims       = IntProperty       (name = "", default = 0            );

    anims          = CollectionProperty(type = ActionPointer              );
    triggers       = CollectionProperty(type = LogicTrigger               );
    active_trigger = IntProperty       (name = "", default = 0            );


register_class(LogicState);

#   ---     ---     ---     ---     ---

ObjectModes =        [
                        ("sprite",    "Sprite",    "A sprite"            ),
                        ("generator", "Generator", "A sprite generator"  )
                     ];

def getObjectStates(self, context): return [("%i"%i, "", context.object.DarkAge.states[i].name) for i in range(30)];

class ObjectSettings(PropertyGroup):

    mode       = EnumProperty      (name = "", items = ObjectModes, default = "sprite");
    curstate   = EnumProperty      (items = getObjectStates                           );
    states     = CollectionProperty(type = LogicState                                 );

#   ---     ---     ---     ---     ---

class SceneSettings(PropertyGroup):

    curobj = PointerProperty(name = "", type = Object);

#   ---     ---     ---     ---     ---

def register():

    Scene. DarkAge = PointerProperty(type = SceneSettings );
    Object.DarkAge = PointerProperty(type = ObjectSettings);

def unregister():

    del Object.DarkAge;
    del Scene. DarkAge;

#   ---     ---     ---     ---     ---