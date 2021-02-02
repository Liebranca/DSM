from .box import DA_UIBox;
from . import gMIN_BOX_HEIGHT;

#   ---     ---     ---     ---     ---

class DA_UIController:
    
    def __init__(self, idex):

        self.idex       = idex;

        self.boxes      = [];
        self.region     = None;

#   ---     ---     ---     ---     ---

    def addBox(self, colle, min_items = gMIN_BOX_HEIGHT, max_items = gMIN_BOX_HEIGHT):
        self.boxes.append( DA_UIBox(colle, len(self.boxes), min_items, max_items) );

#   ---     ---     ---     ---     ---
