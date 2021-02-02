from .prop import GameProperty;
from .trigger import LogicTrigger
from .state import LogicState;

from .xpressprop import Xpressprop;
from .collection import DA_Collection;

import DarkAge as DA

#   ---     ---     ---     ---     ---

class DA_GameBlock:

    def __init__(self, states_from_val = None, props_from_val = None):        

        self.states = DA_Collection(states_from_val, LogicState,   "State"   );
        self.props  = DA_Collection(props_from_val,  GameProperty, "Property");

#   ---     ---     ---     ---     ---

def register():

    gTRIG       = LogicTrigger ("Startup", "Default init method", 0, 0,           vital = 1);

    gMAIN       = LogicState   ("Main",     "Default main state", [gTRIG],        vital = 1);
    gPATH       = GameProperty ("DarkPath", "DarkAge filepath",   2, DA.darkpath, vital = 1);

    DA._globals = DA_GameBlock([gMAIN], [gPATH]);

def unregister():

    del DA._globals;

#   ---     ---     ---     ---     ---
