from .collection import DA_Collection;
from .trigger import LogicTrigger;

#   ---     ---     ---     ---     ---

class LogicState:

    def __init__(self, name = 'State', description = 'A logic state', from_val = None, vital = False):

        self.description = description;
        self.name        = name;

        self.active      = False;
        self.triggers    = DA_Collection(*[from_val, LogicTrigger, "Trigger"]);

        self.vital       = vital;

#   ---     ---     ---     ---     ---
