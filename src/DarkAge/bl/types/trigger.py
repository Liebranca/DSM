#   ---     ---     ---     ---     ---

TriggerTypes =  [ 'PROP', 'INPUT', 'MESSAGE' ];

#   ---     ---     ---     ---     ---

class LogicTrigger:

    def __init__(self, name = "Trigger", description = "A logic trigger", kls = 0, value = 0, vital = False):

        self.name        = name;
        self.description = description;
        self.kls         = TriggerTypes[kls];

        self.value       = value;

    @staticmethod
    def getKlsList(): return TriggerTypes;

#   ---     ---     ---     ---     ---
