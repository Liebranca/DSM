from bpy.types import Operator
from bpy.props import BoolProperty, IntProperty, EnumProperty, StringProperty

from DarkAge.bl.utils.lystr import ensureNameUnique;

from DarkAge.bl.types.prop import PropTypes, read_strList;
from DarkAge.bl.types.prop import read_strDict, morphGamePropValue;

import DarkAge as DA

#   ---     ---     ---     ---     ---

class DA_addGameProp(Operator):

    bl_idname      = "darkage.addgameprop";
    bl_label       = "Add a new property";

    bl_description = "Add a new property to the current object";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        ob            = DA.gbptr;
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

        ob            = DA.gbptr;
        idex          = ob.curprop

        if len(ob.props): ob.props.remove(idex);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---
# propeditor utils

curiter = None; iterop = None;

def update_kls(self): self.update_flags &=~ klsFlag;

def update_ikey(self):

    global curiter;

    curiter.modKey(self.input_key);

    if self.input_key != curiter.value_keys[curiter.ptr]:
        self.input_key = curiter.value_keys[curiter.ptr];

    self.update_flags &=~ ikeyFlag;
    self.update_flags |=  iblockFlag;

def update_ivalue(self):

    if self.kls in ['LIST', 'DICT']:

        global curiter;

        value, key, kls = curiter.getValues();
        v               = baseGamePropTrans(kls, self.input_block);

        if kls         != 'STRING': curiter.modValue(eval(v));
        else                      : curiter.modValue(     v );

    else:
        v = baseGamePropTrans(self.kls, self.input_block);

    if self.input_block != v: self.input_block = v;

    self.update_flags &=~ iblockFlag;

#   ---     ---     ---     ---     ---
# these ops are __mostly__ just one-func-call buttons
# used to manipulate curiter (see DA_iterprop.py)

class DA_iterPropIncSize(Operator):

    bl_idname      = "darkage.ipincsize";
    bl_label       = "Increase array size";

    bl_description = "Add a new item to the array";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global curiter, iterop;
        curiter.extend(); iterop.check(context);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_iterPropDecSize(Operator):

    bl_idname      = "darkage.ipdecsize";
    bl_label       = "Decrease array size";

    bl_description = "Remove selected item from the array";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global curiter, iterop;
        curiter.shorten(); iterop.check(context);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_iterPropAdvPtr(Operator):

    bl_idname      = "darkage.ipadvptr";
    bl_label       = "Moves the selected item down";

    bl_description = "Move this item down";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global curiter, iterop;

        idex = curiter.ptr + 1;
        if idex >= curiter.size:
            idex = 0;

        curiter.positionSwap(idex); curiter.adv_ptr();
        iterop.input_block = str(curiter.curValue());

        iterop.check(context);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_iterPropBakPtr(Operator):

    bl_idname      = "darkage.ipbakptr";
    bl_label       = "Moves the selected item up";

    bl_description = "Move this item up";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global curiter, iterop;

        idex = curiter.ptr - 1;
        if idex < 0:
            idex = curiter.size - 1;

        curiter.positionSwap(idex); curiter.bak_ptr();
        iterop.input_block = str(curiter.curValue());

        iterop.check(context);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_iterPropSelPtr(Operator):

    bl_idname      = "darkage.ipselptr";
    bl_label       = "Move to selected item in array";

    bl_description = "Select this item";

    target         = IntProperty(default = 0);

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global curiter, iterop;

        steps = (self.target - curiter.ptr);

        if steps < 0:
            for i in range(abs(steps)): curiter.bak_ptr();

        else:
            for i in range(steps): curiter.adv_ptr();

        iterop.input_block = str(curiter.curValue());
        iterop.input_key   = str(curiter.curKey  ());

        iterop.check(context);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_iterPropAdvType(Operator):

    bl_idname      = "darkage.ipadvtype";
    bl_label       = "Change value type";

    bl_description = "Change value type of current item in array property";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global curiter, iterop;
        curiter.adv_type(); iterop.check(context);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_iterPropBakType(Operator):

    bl_idname      = "darkage.ipbaktype";
    bl_label       = "Change value type";

    bl_description = "Change value type of current item in array property";

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global curiter, iterop;
        curiter.bak_type(); iterop.check(context);

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

klsFlag  = (1 << 0); iblockFlag = (1 << 1); ikeyFlag   = (1 << 2);

def set_klsFlag   (self, context): self.update_flags |= klsFlag;
def set_iblockFlag(self, context): self.update_flags |= iblockFlag;

def set_ikeyFlag  (self, context):

    global iterop;

    self.update_flags |= ikeyFlag;
    iterop.check(context);

#   ---     ---     ---     ---     ---

def ScrewYou(self, context): return [(kls, '', '') for kls in PropTypes];

class DA_editGameProp(Operator):

    bl_idname      = "darkage.editgameprop";
    bl_label       = "Edit this property";

    bl_description = "Brings up the property edit menu";

    kls            = EnumProperty  (name    = "Type",
                                    items   = ScrewYou,  update  = set_klsFlag                          );

    value          = StringProperty(default = "0"                                                       );
    input_block    = StringProperty(default = "0",       update  = set_iblockFlag                       );
    input_key      = StringProperty(default = "Key",     update  = set_ikeyFlag                         );

    update_flags   = IntProperty   (default = 0                                                         );

#   ---     ---     ---     ---     ---

    def check(self, context):

        global curiter;

        if (self.kls in ['LIST', 'DICT']) and not curiter:
            curiter = Iterprop();

        # managing update flags. actually cleaner than a straight-up update method
        if (self.update_flags & klsFlag   ): update_kls   (self);
        if (self.update_flags & ikeyFlag  ): update_ikey  (self);
        if (self.update_flags & iblockFlag): update_ivalue(self);

        morphGamePropValue(self, context, curiter);

        return True;

#   ---     ---     ---     ---     ---

    def draw(self, context):

        global curiter;

        ob               = DA.gbptr;
        prop             = ob.props[ob.curprop];

        layout           = self.layout;
        row              = layout.row();

        row.prop(prop, "name"); row.prop(self, "kls");

        if   self.kls in ['INT', 'FLOAT', 'STRING', 'BOOL']:
            row = layout.row(); row.label("Value: "); row.prop(self, "input_block", text="");

#   ---     ---     ---     ---     ---

        elif self.kls in ['LIST', 'DICT']:

            layout.separator();

            row   = layout.row(); box = row.box();
            split = box.split(0.1);

            col1, col2, col3, col4 = (split.column(True), split.column(True), split.column(True), split.column(True));

            fil   = 3 - curiter.size;

#   ---     ---     ---     ---     ---

            for i in range(curiter.size):

                is_cur_item = (i == curiter.ptr);

                ic          = "RADIOBUT_ON" if is_cur_item else "RADIOBUT_OFF";
                oppy        = col1.operator("darkage.ipselptr", text = "", icon = ic, depress = is_cur_item);
                oppy.target = i;

#   ---     ---     ---     ---     ---

                if is_cur_item:
                    
                    if self.kls == 'DICT':
                        col2.prop(self, "input_key", text="");

                    else:
                        col2.label("%i"%i);

                    sub = col3.row(True); sub.alignment = 'LEFT';

                    sub.operator("darkage.ipbaktype", text="", icon="TRIA_LEFT");
                    sub.operator("darkage.ipadvtype", text="", icon="TRIA_RIGHT");

                    sub.label("%s"%curiter.value_types[i]);

                    col4.prop(self, "input_block", text="");

#   ---     ---     ---     ---     ---

                else:
                    if self.kls == 'DICT':
                        col2.label("%s"%curiter.value_keys[i]);

                    else:
                        col2.label("%i"%i);

                    col3.label("%s"%curiter.value_types[i]  );
                    col4.label("%s"%(str(curiter.values[i])));

            if fil > 0:
                for i in range(fil): col1.label("");

#   ---     ---     ---     ---     ---

            col = row.column(True);
            col.operator("darkage.ipincsize", text="", icon="ZOOMIN"    );
            col.operator("darkage.ipdecsize", text="", icon="ZOOMOUT"   );
            col.operator("darkage.ipbakptr",  text="", icon="TRIA_UP"   );
            col.operator("darkage.ipadvptr",  text="", icon="TRIA_DOWN" );

#   ---     ---     ---     ---     ---

    def execute(self, context): return {'FINISHED'};

#   ---     ---     ---     ---     ---

    def invoke(self, context, event):

        ob = DA.gbptr;
        if not len(ob.props): return {'CANCELLED'};

        global curiter, iterop;

        if curiter:
            del curiter;
            curiter = None;

        iterop = self;

#   ---     ---     ---     ---     ---

        prop = ob.props[ob.curprop];
        if(self.kls != prop.kls): self.kls = prop.kls;

        if self.kls in ['INT', 'FLOAT', 'STRING', 'BOOL']:
            self.value = prop.value;

#   ---     ---     ---     ---     ---

        elif self.kls == 'LIST':

            value = read_strList(prop.value);

            if value:
                curiter = Iterprop(value);
                setattr(self, "input_block", str(curiter.values[0]));                    

            else:
                curiter = Iterprop();

#   ---     ---     ---     ---     ---

        elif self.kls == 'DICT':

            value = read_strDict(prop.value);

            if value:
                curiter = Iterprop(value);
                setattr(self, "input_block", str(curiter.values    [0]));
                setattr(self, "input_key",   str(curiter.value_keys[0])); 

            else:
                curiter = Iterprop();

#   ---     ---     ---     ---     ---

        wm = context.window_manager;
        return wm.invoke_props_dialog(self, width=600);

#   ---     ---     ---     ---     ---
