#   ---     ---     ---     ---     ---
# expression properties are special strings that contain python code;
# they are passed through an eval() to generate a value

#   ---     ---     ---     ---     ---

"""

expression prop syntax:

    g. -> to reference globals                   (defined in scene panel                                )
    o. -> to reference current object            (either KX_GameObject or subclass                      )
    s. -> to reference current object scene      (shorthand; s.value == o.scene.value                   )
    c. -> to reference current object component  (shorthand; o.components[name] == c.name               )
    p. -> to reference current object properties (from game property dict; own["prop"]                  )

examples:

    remember these are __shortcuts__ to data. you can do with that whatever python will let you.

    1. "g.some_global"
        value of property is equal to some_global

    2. "p.some_prop == g.some_global"
        value of property is equal to the result of this evaluation

    3. "(p.some_prop * p.other_prop) + p.another_prop"
        value of property is equal to the result of this operation

    is comparing or mashing values all we can do? nope! we can call functions too.

    1. "o.func(args)"
        will call own.func and pass args; value of property will be the return of the function

    2. "o.func(p.some_prop)"
        will call function and pass value of property some_prop

    3. "s.objects[name].func(p.some_prop)"
        will look for object by name and call func

"""

#   ---     ---     ---     ---     ---

subj_dict = {"g.": "bpy.DarkAge", "s.": "context.scene.DarkAge"}

#   ---     ---     ---     ---     ---

class Xpressprop:

    def __init__(self, from_val = None):


        if from_val:
            pass;

#   ---     ---     ---     ---     ---

        else:

            self.subject = 'g.';
            self.attr    = 'props';
            self.value   = 'Property';
            self.args    = '';

#   ---     ---     ---     ---     ---
# this method shows the expression in human-readable form (for bpy ui)

    def translate(self, context):

        import bpy;

        s = subj_dict[self.subject] + "." + self.attr;
        if self.value:
             s = s + ("[%s]"%self.value);

        if self.args:
            s = s + ("(%s)"%self.args);

        return eval(s);

#   ---     ---     ---     ---     ---
# this method reads the expression and returns the new interpretation (used by bge)

    def evaluate(self):
        pass;
