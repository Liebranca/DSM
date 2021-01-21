import bpy

from bpy.types import Operator
from bpy.props import BoolProperty, IntProperty, EnumProperty, StringProperty
from bpy.utils import register_class, unregister_class

from .DA_utils import ensureNameUnique, PropTypes, BasePropTypes, read_strList, baseGamePropTrans, morphGamePropValue;

#   ---     ---     ---     ---     ---

class DA_objectInit(Operator):

    bl_idname      = "darkage.objectinit";
    bl_label       = "Initializer for DarkAge";

    bl_description = "An object initializer necessary to bypass the utter madness of this API";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob = context.object.DarkAge;

        for i in range(30):
            new_state = ob.states.add();
            new_state.name = "State %i"%i;

        self.report({'INFO'}, "Object was initialized");
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_addGameProp(Operator):

    bl_idname      = "darkage.addgameprop";
    bl_label       = "Add a new property";

    bl_description = "Add a new property to the current object";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob            = context.object.DarkAge;
        new_prop      = ob.props.add();
        new_prop.idex = len(ob.props) - 1;

        new_prop.name = ensureNameUnique(ob.props, new_prop.idex);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_delGameProp(Operator):

    bl_idname      = "darkage.delgameprop";
    bl_label       = "Remove selected property";

    bl_description = "Remove active property from current object";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob            = context.object.DarkAge;
        idex          = ob.curprop

        if len(ob.props): ob.props.remove(idex);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_editGameProp(Operator):

    bl_idname      = "darkage.editgameprop";
    bl_label       = "Edit this property";

    bl_description = "Brings up the property edit menu";

    kls            = EnumProperty  (update  = morphGamePropValue, name    = "Type",
                                    default = "INT",              items   = PropTypes    );

    slot0          = StringProperty(update  = morphGamePropValue, default = "0"          );

    slot1          = StringProperty(default = "0"                                        );
    slot2          = StringProperty(default = "0"                                        );
    slot3          = StringProperty(default = "0"                                        );
    slot4          = StringProperty(default = "0"                                        );
    slot5          = StringProperty(default = "0"                                        );
    slot6          = StringProperty(default = "0"                                        );
    slot7          = StringProperty(default = "0"                                        );
    slot8          = StringProperty(default = "0"                                        );
    

    lval_num       = IntProperty   (name    = "List size", default = 2, min  = 2, max = 8);
    lval_1         = EnumProperty  (default = "INT",       items   = BasePropTypes       );
    lval_2         = EnumProperty  (default = "INT",       items   = BasePropTypes       );
    lval_3         = EnumProperty  (default = "INT",       items   = BasePropTypes       );
    lval_4         = EnumProperty  (default = "INT",       items   = BasePropTypes       );
    lval_5         = EnumProperty  (default = "INT",       items   = BasePropTypes       );
    lval_6         = EnumProperty  (default = "INT",       items   = BasePropTypes       );
    lval_7         = EnumProperty  (default = "INT",       items   = BasePropTypes       );
    lval_8         = EnumProperty  (default = "INT",       items   = BasePropTypes       );

    listUpdated    = BoolProperty  (default = True                                       );


#   ---     ---     ---     ---     ---

    def check(self, context):

        # take advantage of this redraw to display the correct values
        if self.kls == 'LIST':

            for i in range(1, self.lval_num+1):
                subkls = getattr(self, "lval_%i"%i);
                subval = getattr(self, "slot%i"%i);

                value = baseGamePropTrans(subkls, subval);
                if value != subval: setattr(self, "slot%i"%i, value);

            self.listUpdated = False
            morphGamePropValue(self, context);

        return True;

    def draw(self, context):

        ob               = context.object.DarkAge;
        prop             = ob.props[ob.curprop];

        layout           = self.layout;
        row              = layout.row();

        row.prop(prop, "name"); row.prop(self, "kls");

        if   self.kls in ['INT', 'FLOAT', 'STRING', 'BOOL']:
            row = layout.row(); row.label("Value: "); row.prop(self, "slot0", text="");

        elif self.kls == 'LIST':
            row = layout.row(); row.prop(self, "lval_num");

            for i in range(1, self.lval_num+1):

                row = layout.row(); row.prop(self, "lval_%i"%i, text="");
                row.label("Value: "); row.prop(self, "slot%i"%i, text="");

    def execute(self, context): return {'FINISHED'};

    def invoke(self, context, event):

        ob = context.object.DarkAge;
        if not len(ob.props): return {'CANCELLED'};

        prop = ob.props[ob.curprop];
        if(self.kls != prop.kls): self.kls = prop.kls;

        if self.kls in ['INT', 'FLOAT', 'STRING', 'BOOL']:
            self.slot0 = prop.value;

        elif self.kls == 'LIST':

            value = read_strList(prop.value);
            if value:

                i = 1;
                for x in value:
                    setattr(self, "slot%i"%i, str(x)); i += 1;

                self.lval_num = len(value);

        wm = context.window_manager;
        return wm.invoke_props_dialog(self, width=600);

#   ---     ---     ---     ---     ---

class DA_addTrigger(Operator):

    bl_idname      = "darkage.addtrigger";
    bl_label       = "Add a new trigger";

    bl_description = "Adds a new trigger to the current state";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob               = context.object.DarkAge;
        state            = ob.states[int(ob.curstate)];
        new_trigger      = state.triggers.add();
        new_trigger.idex = len(state.triggers) - 1;

        new_trigger.name = ensureNameUnique(state.triggers, new_trigger.idex);

        return {'FINISHED'}

class DA_delTrigger(Operator):

    bl_idname      = "darkage.deltrigger";
    bl_label       = "Removes a trigger";

    bl_description = "Removes active trigger from current state";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob          = context.object.DarkAge;
        state       = ob.states[int(ob.curstate)];

        if len(state.triggers): state.triggers.remove(state.active_trigger);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class SB_editTrigger(Operator):

    bl_idname      = "darkage.edittrigger";
    bl_label       = "Edit selected trigger";

    bl_description = "Brings up the trigger editing pop-up";

#   ---     ---     ---     ---     ---

    def draw(self, context):

        ob           = context.object.DarkAge;
        state        = ob.states[int(ob.curstate)];
        trigger      = state.triggers[state.active_trigger];

        layout       = self.layout;
        row          = layout.row();

        row.prop(trigger, "name"); row.prop(trigger, "kls");

#   ---     ---     ---     ---     ---

        layout.separator();

        if trigger.kls == "mess":
            pass;

#   ---     ---     ---     ---     ---

    def execute(self, context): return {'FINISHED'};

    def invoke(self, context, event):

        ob           = context.object.DarkAge;
        state        = ob.states[int(ob.curstate)];

        if not len(state.triggers): return {'CANCELLED'};

        wm = context.window_manager;
        return wm.invoke_props_dialog(self, width=600);

#   ---     ---     ---     ---     ---

def register(): pass;   

def unregister(): pass;