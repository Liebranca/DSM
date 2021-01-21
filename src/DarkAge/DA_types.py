from bpy.types import Property, Scene, Object, Action, Material, PropertyGroup;
from bpy.props import StringProperty, EnumProperty, FloatProperty, IntProperty, PointerProperty;
from bpy.props import BoolProperty, CollectionProperty;

from bpy.utils import register_class;

from .DA_utils import ensureNameUnique, PropTypes;

#   ---     ---     ---     ---     ---

def uNameWrap_GameProp(self, context):

    ob                = context.object.DarkAge
    props             = ob.props;

    unique_name       = ensureNameUnique(props, self.idex);

    if unique_name != self.name:
        self.name = unique_name;

class GameProperty(PropertyGroup):

    name  = StringProperty (name = "Name",  default = "Property", update = uNameWrap_GameProp);
    kls   = EnumProperty   (name = "Type",  default = "INT",      items  = PropTypes         );
    value = StringProperty (name = "Value", default = "0",                                   );
    idex  = IntProperty    (                                                                 );

register_class(GameProperty);

#   ---     ---     ---     ---     ---

class ActionPointer(PropertyGroup):
    val = PointerProperty(type = Action);

register_class(ActionPointer);

#   ---     ---     ---     ---     ---

TriggerTypes =  [
                    ("PROP",    "Property", "Activates through property evaluation"),
                    ("INPUT",   "Input",    "Activates via input"                  ),
                    ("MESSAGE", "Message",  "Activates via messages"               ),
                ];

def uNameWrap_Trigger(self, context):

    ob                = context.object.DarkAge
    state             = ob.states[int(ob.curstate)];

    unique_name       = ensureNameUnique(state.triggers, self.idex);

    if unique_name != self.name:
        self.name = unique_name;    

class LogicTrigger(PropertyGroup):

    name         = StringProperty    (name = "Name", default = "Trigger", update = uNameWrap_Trigger);
    kls          = EnumProperty      (name = "Type", items  = TriggerTypes, default = "PROP"        );
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
                        ("SPRITE",    "Sprite",    "A sprite"            ),
                        ("GENERATOR", "Generator", "A sprite generator"  )
                     ];

def getObjectStates(self, context): return [("%i"%i, "", context.object.DarkAge.states[i].name) for i in range(30)];
def getObjectProps(self): pass;

class ObjectSettings(PropertyGroup):

    mode       = EnumProperty      (name = "", items = ObjectModes, default = "SPRITE");
    curstate   = EnumProperty      (items = getObjectStates                           );
    states     = CollectionProperty(type = LogicState                                 );
    props      = CollectionProperty(type = GameProperty                               );
    curprop    = IntProperty       (                                                  );

#   ---     ---     ---     ---     ---

class SceneSettings(PropertyGroup):

    curobj = PointerProperty(name = "", type = Object);

#   ---     ---     ---     ---     ---

def register():

    Scene. DarkAge = PointerProperty(type = SceneSettings );
    Object.DarkAge = PointerProperty(type = ObjectSettings);

def unregister():

    unregister_class(GameIntProperty);
    unregister_class(GameProperty   );
    unregister_class(ActionPointer  );
    unregister_class(LogicTrigger   );
    unregister_class(LogicState     );

    del Object.DarkAge;
    del Scene. DarkAge;

#   ---     ---     ---     ---     ---