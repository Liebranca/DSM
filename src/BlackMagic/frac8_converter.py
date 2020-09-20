import bpy

# - --- - -- - --- -

f8_size   = 128;
f8_step   = 1/32;
f8_maxval = (128 - 1) * f8_step;

f8_range  = [i * f8_step for i in range(f8_size)];

uf8_size  = 256;
uf8_step = 0.00390625;

uf8_range = [i * uf8_step for i in range(uf8_size)];

# - --- - -- - --- -

def approadf(v1, v2, m):
    if ((v2 - m) < v1) and (v1 < (v2 + m)):
        return max(v1, v2) - min(v1, v2);
    
    return 999;

def take_closest(x, fvr, step):

    s       = step * 0.5;
    dist    = 999;
    newdist = 0;
    closest = 0;

    for val in fvr:
        newdist = approadf(x, val, s);
        
        if newdist < dist:
            dist    = newdist;
            closest = val;
            
    return closest;
        

def ftf8(x):
    sign = 1 if x < 0 else 0;
    if abs(x) >= f8_maxval: return f8_maxval * (1 - (sign * 2));

    return take_closest(abs(x), f8_range, f8_step) * (1 - (sign * 2));

def ftuf8(x):
    if   x >= 1.0: return 1.0;
    elif x <= 0.0: return 0.0;

    return take_closest(x, uf8_range, uf8_step);

# - --- - -- - --- -

def meshfrac(ob_name):

    ob = None;
    if ob_name in bpy.context.scene.objects:
        ob = bpy.context.scene.objects[ob_name];

    else:
        print("Object %s not found.\n"%ob_name)
        return 0;

    me = ob.data;

    for vert in me.vertices:
        for axis in range(3):
            vert.co[axis] = ftf8(vert.co[axis]);

    uv = me.uv_layers.active.data;
    for face in me.polygons:
            for loop_index in face.loop_indices:
                uv[loop_index].uv[0] = ftuf8(uv[loop_index].uv[0]);
                uv[loop_index].uv[1] = ftuf8(uv[loop_index].uv[1]);
