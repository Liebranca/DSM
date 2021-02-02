from ..ops import colle as DA_OPS_Collection;
from . import gMIN_BOX_HEIGHT;

import DarkAge as DA;

#   ---     ---     ---     ---     ---

#custom UIList to view and manage DA_Collections

class DA_UIBox:

    def __init__(self, colle, idex, min_items = gMIN_BOX_HEIGHT, max_items = gMIN_BOX_HEIGHT):

        self.colle     = colle;
        self.idex      = idex;

        self.scroll_h  = 0;

        self.min_items = min_items;
        self.max_items = max_items;

    def scrollStep(self, dirn):
        step = 3 * dirn;
        self.scroll_h = min(max(0, self.scroll_h + step), self.colle.size - 1);

    def draw(self, context, layout, cont_idex):

        row    = layout.row(); box = row.box();

        sub1   = box.split (0.10, True); col1 = sub1.column(True);
        sub2   = sub1.split(0.25, True); col2 = sub2.column(True);
        sub3   = sub2.split(0.95, True); col3 = sub3.column(True);
        sub4   = sub3.split(1.00, True); kcol = sub4.column(True);

        fil   = self.max_items - self.colle.size;
        i     = self.scroll_h;

#   ---     ---     ---     ---     ---

        for j in range(self.max_items):

            if i >= self.colle.size: break;

            v               = self.colle[i];
            is_cur_item     = (i == self.colle.ptr);

            ic              = "RADIOBUT_ON" if is_cur_item else "RADIOBUT_OFF";
            oppy            = col1.operator("darkage.collesel", text="", icon = ic, depress = is_cur_item);

            oppy.cont_idex  = cont_idex;
            oppy.box_idex   = self.idex;
            oppy.target     = i;

            i              += 1;

            col2.label("%s:"%v.name );
            col3.label(" %s"%v.value);

        if fil > 0:
            for i in range(fil): col1.label("");

        if self.colle.size > self.max_items:

            midpoint  = int(self.max_items / 2 );
            barpos    = min(max(1, int( (self.scroll_h) / (self.colle.size / self.max_items)) ), self.max_items - 2);

            smoothbar = (barpos / (1 + self.scroll_h));

            for i in range(self.max_items):

                ic = "BLANK1";

                if   i == 0                 : ic = "TRIA_UP_BAR";
                elif i == self.max_items - 1: ic = "TRIA_DOWN_BAR";

                elif i == barpos:
                    if smoothbar  >= 0.51   : ic = "DOTSUP";
                    else                    : ic = "DOTSDOWN";

                oppy   = kcol.operator("darkage.collescroll", text="", icon = ic, emboss = True);

                oppy.box_idex  = self.idex;
                oppy.cont_idex = cont_idex;
                oppy.target    = 1 if i >= barpos else -1;

#   ---     ---     ---     ---     ---

        col = row.column(True); col.alignment = "LEFT";

        oppy            = col.operator ("darkage.colleadd",  text="", icon="ZOOMIN"    );
        oppy.cont_idex  = cont_idex;
        oppy.box_idex   = self.idex;

        oppy            = col.operator ("darkage.collepop",  text="", icon="ZOOMOUT"   );
        oppy.cont_idex  = cont_idex;
        oppy.box_idex   = self.idex;

        oppy            = col.operator ("darkage.colleedit", text="", icon="SCRIPTWIN" );
        oppy.cont_idex  = cont_idex;
        oppy.box_idex   = self.idex;

        col.separator(                                               );

        oppy            = col.operator ("darkage.colleup",   text="", icon="TRIA_UP"   );
        oppy.cont_idex  = cont_idex;
        oppy.box_idex   = self.idex;

        oppy            = col.operator ("darkage.colledown", text="", icon="TRIA_DOWN" );
        oppy.cont_idex  = cont_idex;
        oppy.box_idex   = self.idex;

        row             = layout.split(1.0, True); row.alignment = 'EXPAND'; row.scale_x = 1.0;
        oppy            = row.operator("darkage.collegrip", icon='DOWNARROW_HLT', emboss = True);
        oppy.cont_idex  = cont_idex;
        oppy.box_idex   = self.idex;

#   ---     ---     ---     ---     ---