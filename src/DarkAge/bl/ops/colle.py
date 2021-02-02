from bpy.types import Operator
from bpy.props import StringProperty, IntProperty, FloatProperty, BoolProperty;

from ..ui import gMIN_BOX_HEIGHT;

import inspect;
import DarkAge as DA;

#   ---     ---     ---     ---     ---

class DA_ColleScroll(Operator):

    bl_idname      = "darkage.collescroll";
    bl_label       = "";

    bl_description = "Click to scroll";

    box_idex       = IntProperty  (default = 0    );
    cont_idex      = IntProperty  (default = 0    );

    target         = IntProperty  (default = -1   );

#   ---     ---     ---     ---     ---

    def execute(self, context):

        box = DA.conts[self.cont_idex].boxes[self.box_idex]; box.scrollStep(self.target);
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_ColleGrip(Operator):

    bl_idname      = "darkage.collegrip";
    bl_label       = "";

    bl_description = "Click and release to grip. Click again to let go";

    box_idex       = IntProperty  (default = 0    );
    cont_idex      = IntProperty  (default = 0    );

    mouse_y        = IntProperty  (default = 0    );
    start_max      = IntProperty  (default = 0    );
    acumdelta      = FloatProperty(default = 0.0  );

#   ---     ---     ---     ---     ---

    def modal(self, context, event):

        box = DA.conts[self.cont_idex].boxes[self.box_idex];

        if event.type == 'MOUSEMOVE':

            delta           = (self.mouse_y - event.mouse_y) * 0.05;
            self.acumdelta +=  delta;

            box.max_items   = max(gMIN_BOX_HEIGHT, self.start_max + int(self.acumdelta));
            self.mouse_y    = event.mouse_y;

            context.area.tag_redraw();

        elif event.type == 'LEFTMOUSE':
            return {'FINISHED'}

#   ---     ---     ---     ---     ---            

        return {'RUNNING_MODAL'}

#   ---     ---     ---     ---     ---

    def invoke(self, context, event):

        self.mouse_y   = event.mouse_y; self.acumdelta = 0.0;
        self.start_max = DA.conts[self.cont_idex].boxes[self.box_idex].max_items;

        context.window_manager.modal_handler_add(self)

        return {'RUNNING_MODAL'}

#   ---     ---     ---     ---     ---

class DA_ColleAdd(Operator):

    bl_idname      = "darkage.colleadd";
    bl_label       = "Increase array size";

    bl_description = "Add a new item to the array";

    box_idex       = IntProperty  (default = 0    );
    cont_idex      = IntProperty  (default = 0    );

    def execute(self, context):

        DA.conts[self.cont_idex].boxes[self.box_idex].colle.extend();
        return {'FINISHED'};

#   ---     ---     ---     ---     ---

class DA_CollePop(Operator):

    bl_idname      = "darkage.collepop";
    bl_label       = "Decrease array size";

    bl_description = "Remove selected item from the array";

    box_idex       = IntProperty   (default = 0);
    cont_idex      = IntProperty   (default = 0);

    def execute(self, context):

        colle = DA.conts[self.cont_idex].boxes[self.box_idex].colle;
        if colle.shorten() == 1:
            self.report({'WARNING'}, "Don't you dare delete %s, pippin!"%colle.curValue().name)

        return {'FINISHED'};

#   ---     ---     ---     ---     ---

class DA_ColleSelect(Operator):

    bl_idname      = "darkage.collesel";
    bl_label       = "Move to selected item in array";

    bl_description = "Select this item";

    target         = IntProperty  (default = 0);
    box_idex       = IntProperty  (default = 0);
    cont_idex      = IntProperty  (default = 0);

#   ---     ---     ---     ---     ---

    def execute(self, context):

        colle = DA.conts[self.cont_idex].boxes[self.box_idex].colle;
        steps = (self.target - colle.ptr);

        if steps < 0:
            for i in range(abs(steps)): colle.bak_ptr();

        else:
            for i in range(steps): colle.adv_ptr();

        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_ColleMoveDown(Operator):

    bl_idname      = "darkage.colledown";
    bl_label       = "Moves the selected item down";

    bl_description = "Move this item down";

    box_idex       = IntProperty  (default = 0    );
    cont_idex      = IntProperty  (default = 0    );

#   ---     ---     ---     ---     ---

    def execute(self, context):

        colle = DA.conts[self.cont_idex].boxes[self.box_idex].colle;
        idex = colle.ptr + 1;
        if idex >= colle.size:
            idex = 0;

        colle.positionSwap(idex); colle.adv_ptr();
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_ColleMoveUp(Operator):

    bl_idname      = "darkage.colleup";
    bl_label       = "Moves the selected item up";

    bl_description = "Move this item up";

    box_idex       = IntProperty  (default = 0    );
    cont_idex      = IntProperty  (default = 0    );

#   ---     ---     ---     ---     ---

    def execute(self, context):

        colle = DA.conts[self.cont_idex].boxes[self.box_idex].colle;
        idex = colle.ptr - 1;
        if idex < 0:
            idex = colle.size - 1;

        colle.positionSwap(idex); colle.bak_ptr();
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

# helper class

class Colle_EditHelper:

    def __init__(self, ob, ophandle):

        self.ob    = ob; self.ptr = 0;
        self.props = type(ob).getEditValues(ob);

        self.value_idex = 0;
        for prop in self.props:
            if prop.key == 'value': break;

            self.value_idex += 1;

        self.ophandle = ophandle;

    def __iter__   (self     ): return self.props.__iter__();
    def __getitem__(self, key): return self.props[key];

#   ---     ---     ---     ---     ---

    def getCurr(self             ): return self.props[self.ptr];
    def modProp(self, idex, value):

        prop          = self.props[idex];

        if prop.kls  != 'STRING':

            kls       = prop.kls;
            morphFunc = DA.inputMorph[kls]

#   ---     ---     ---     ---     ---

            if kls   == 'PROP': kls = self.ob.kls;

            sig       = inspect.signature(morphFunc);
            args      = [None, None, None];

            if "x"        in sig.parameters: args[0] = str(value);
            if "kls"      in sig.parameters: args[1] = kls;
            if "subcolle" in sig.parameters: args[2] = [0];

#   ---     ---     ---     ---     ---

            args  = [arg for arg in args if arg];
            value = morphFunc(*args);

#   ---     ---     ---     ---     ---

        prop.value                  = value;
        self.ob.__dict__[prop.key]  = value;

    def modCurr(self, value): self.modProp(self.ptr, value);

#   ---     ---     ---     ---     ---
# helper class instace
gCOLLE_HELPER = None;

class DA_ColleAdvType(Operator):

    bl_idname      = "darkage.colleadvtype";
    bl_label       = "Switch to next item in enum";

    bl_description = "Go to next item in enum";

    prop_idex      = IntProperty  (default = 0    );

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global gCOLLE_HELPER; helper = gCOLLE_HELPER;
        prop = helper.props[self.prop_idex]; prop.kls.adv_type();

        helper.ob.kls = prop.kls.current;
        helper.modProp(helper.value_idex, helper.props[helper.value_idex].value);
        helper.ophandle.check(context);
        
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_ColleBakType(Operator):

    bl_idname      = "darkage.collebaktype";
    bl_label       = "Switch to previous item in enum";

    bl_description = "Go to previous item in enum";

    prop_idex      = IntProperty  (default = 0    );

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global gCOLLE_HELPER; helper = gCOLLE_HELPER;
        prop = helper.props[self.prop_idex]; prop.kls.bak_type();

        helper.ob.kls = prop.kls.current;
        helper.modProp(helper.value_idex, helper.props[helper.value_idex].value);
        helper.ophandle.check(context);
        
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_ColleSubSel(Operator):

    bl_idname      = "darkage.collesubsel";
    bl_label       = "Select item for input";

    bl_description = "Select this item";

    prop_idex      = IntProperty  (default = 0    );

#   ---     ---     ---     ---     ---

    def execute(self, context):

        global gCOLLE_HELPER; helper = gCOLLE_HELPER;
        helper.ptr = self.prop_idex;
        
        return {'FINISHED'}

#   ---     ---     ---     ---     ---

class DA_ColleEdit(Operator):

    bl_idname      = "darkage.colleedit";
    bl_label       = "Edit this item";

    bl_description = "Edit selected item";

    box_idex       = IntProperty   (default = 0    );
    cont_idex      = IntProperty   (default = 0    );

    mouse_x        = IntProperty   (default = 0    );
    mouse_y        = IntProperty   (default = 0    );
    sneaky_mouse   = BoolProperty  (default = False);

    input_block    = StringProperty(               );

#   ---     ---     ---     ---     ---

    def check(self, context):

        global gCOLLE_HELPER; helper = gCOLLE_HELPER;

        if str(helper.getCurr().value) != self.input_block:
            helper.modCurr(self.input_block);

        DA.conts[self.cont_idex].region.tag_redraw();

        return True;

#   ---     ---     ---     ---     ---

    def draw(self, context):

        global gCOLLE_HELPER; helper = gCOLLE_HELPER;

        if not self.sneaky_mouse:
            self.sneaky_mouse = True;
            context.window.cursor_warp(self.mouse_x, self.mouse_y);

        colle  = DA.conts[self.cont_idex].boxes[self.box_idex].colle;
        layout = self.layout;

        row    = layout.row(); box = row.box();

        sub1   = box.split (0.10, True); col1 = sub1.column(True);
        sub2   = sub1.split(0.25, True); col2 = sub2.column(True);
        sub3   = sub2.split(0.95, True); col3 = sub3.column(True);
        sub4   = sub3.split(1.00, True); kcol = sub4.column(True);

        i      = 0;

#   ---     ---     ---     ---     ---

        self.input_block = str(helper.getCurr().value);

        for prop in helper.props:

            if i == helper.value_idex:
                if (i + 1) >= len(helper.props): break;
                i += 1; prop = helper.props[i];

            kls, key, value   = prop.getVars();
            is_cur_item = (i == helper.ptr);

#   ---     ---     ---     ---     ---

            if kls == 'KLS':

                col1.separator(); col2.separator(); col3.separator();

                col2.label("Type: ");

                split = col1.split(0.5, True);

                oppy = split.operator("darkage.collebaktype", text="", icon = "TRIA_LEFT" );
                oppy.prop_idex = i;

                oppy = split.operator("darkage.colleadvtype", text="", icon = "TRIA_RIGHT");
                oppy.prop_idex = i;

                col3.label(key);

                col1.separator(); col2.separator(); col3.separator();

#   ---     ---     ---     ---     ---

            else:

                ic = "RADIOBUT_ON" if is_cur_item else "RADIOBUT_OFF";

                oppy = col1.operator("darkage.collesubsel", text="", icon = ic, depress = is_cur_item)
                oppy.prop_idex = i;

                col2.label(key);

#   ---     ---     ---     ---     ---

                if is_cur_item:
                    col3.prop(self, "input_block", text = "");

                else:
                    col3.label(value);

            i += 1;

#   ---     ---     ---     ---     ---

        #layout.separator();

        row             = layout.row  (          ); box  = row. box   (    );

        sub1            = box.   split(0.10, True); col1 = sub1.column(True);
        sub2            = sub1.  split(0.25, True); col2 = sub2.column(True);
        sub3            = sub2.  split(0.95, True); col3 = sub3.column(True);
        sub4            = sub3.  split(1.00, True); kcol = sub4.column(True);

        prop            = helper.props[helper.value_idex];

        kls, key, value = prop.getVars(); kls = helper.ob.kls;
        is_cur_item     = (helper.value_idex == helper.ptr);

#   ---     ---     ---     ---     ---

        if kls in ['INT', 'FLOAT', 'BOOL', 'STRING']:

            ic             = "RADIOBUT_ON" if is_cur_item else "RADIOBUT_OFF";

            oppy           = col1.operator("darkage.collesubsel", text="", icon = ic, depress = is_cur_item)
            oppy.prop_idex = helper.value_idex; col2.label(key);

            if is_cur_item:
                col3.prop(self, "input_block", text = "");

            else:
                col3.label(value);

#   ---     ---     ---     ---     ---

        if kls in ['LIST', 'DICT']:
            pass;
            """
            for i in range(some_size):
                oppy           = col1.operator("darkage.collesubsel", text="", icon = ic, depress = is_cur_item)
                oppy.prop_idex = i; col2.label(key);
            """

#   ---     ---     ---     ---     ---

        else:
            pass;

#   ---     ---     ---     ---     ---

    def execute(self, context):
        return {'FINISHED'}

    def invoke(self, context, event):

        colle = DA.conts[self.cont_idex].boxes[self.box_idex].colle;
        DA.conts[self.cont_idex].region = context.region;

        global gCOLLE_HELPER;
        if gCOLLE_HELPER: del gCOLLE_HELPER;

        gCOLLE_HELPER = Colle_EditHelper(colle.curValue(), self);

        self.mouse_x, self.mouse_y, self.sneaky_mouse = event.mouse_x, event.mouse_y, False;
        context.window.cursor_warp(context.window.width - context.region.width - 300, (context.region.height));

        wm = context.window_manager; 
        return wm.invoke_props_dialog(self, width = 600);

#   ---     ---     ---     ---     ---
