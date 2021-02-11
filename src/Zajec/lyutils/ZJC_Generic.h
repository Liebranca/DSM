#ifndef __ZJC_GENERIC_H__
#define __ZJC_GENERIC_H__

#include "../ZJC_CommonTypes.h"

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
extern "C" {
#endif

#define GENFUNC_DECL(T, name, ide, ...) T name##ide##(__VA_ARGS__)
#define GENFUNC_DEFN(T, name, ide, body, ...) T name##ide##(__VA_ARGS__) { body }

#define GENSTRUCT_DECL(T, name, ide, fields, methods) typedef struct T##_##name { fields } ide##name; methods

#define GENFUNC_DECL2(T, mod, desc, name, ...) T mod##desc##name##(__VA_ARGS__)
#define GENFUNC_DEFN2(T, mod, desc, name, body, ...) T mod##desc##name##(__VA_ARGS__) { body }

//  ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_GENERIC_H__
