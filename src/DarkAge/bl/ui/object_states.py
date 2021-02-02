import DarkAge as DA

#   ---     ---     ---     ---     ---

def drawStateBlock(layout):

    layout.separator();

    ob            = DA.gbptr;

    curstate      = int            (ob.curstate);
    state         = ob.states      [curstate   ];

    row           = layout.row(); row.label("Current: %s"%state.name);
    row           = layout.row();

    col           = row.column_flow(17, True);

    col.scale_x   = 0.75;
    col.scale_y   = 0.75;

#   ---     ---     ---     ---     ---

    for i in range(15):

        if i and not (i%5):
            col.label(""); col.label("");

        for j in [i, i+15]:

            ic = 'NONE';

            if ob.states[j].used:
                ic = 'LAYER_USED';
                if int(curstate) == j: ic = 'LAYER_ACTIVE';

            col.prop_enum(ob, "curstate", "%i"%j, text = "", icon = ic);

#   ---     ---     ---     ---     ---

    layout.separator();

    row = layout.row();

    row.prop(state, "used"); row.label("ON" if state.used else "OFF");
    row.prop(state, "name");

#   ---     ---     ---     ---     ---
