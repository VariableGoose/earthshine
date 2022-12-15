# Earthshine
[Insert logo here]  
A general purpose library written in pedantic c99. It's incompatable with MSVC because it doesn't implement `__typeof__` which is used quite a bit in macros.

## Usage
Copy [es.h](./es.h) into your project. Include it and define ES_IMPL in **ONE** source file.
```c
#define ES_IMPL
#include <es.h>
```

# Features
- [x] Dynamic array
- [x] Hash table
- [ ] Windowing
- [ ] Graphics
- [x] File operations
- [ ] Math
- [ ] Logging
- [ ] Memory debugging
- [x] Assertions
- [x] Threading
- [ ] Strings
- [ ] Library loading
- [ ] Error handler

# Convention
Everything is written in lowercase with underscores (some exceptions apply).

## Types
Types will be suffixed with a `_t`.  
Other than basic types, such as ints and floats, the types will be prefixed with es as a namespace.

Example:
```c
i32_t everything = 42;
f32_t pi = 3.1415926f;
es_type_t foo = {0};
```

## Functions
Functions are prefixed with es as a namespace.  
Functions are also prefixed with it's operational type such as `es_type_operation`.

An internal function will be prefixed with an underscore. If the function is a lower level macro then it's suffixed with a `_impl`.

Example:
```c
// External function.
es_type_t es_type_init(i32_t foo, b8_t bar);
// Internal function.
void _es_type_private(es_type_t *type);
```

## Macros
Macro functions only difference from regular functions are their argument names which are uppercase and usually shortened.  
Macro values are all uppercase to signify a constant value.

Example:
```c
#define es_macro_function(F, B) do { es_type_init((F), (B)); } while(0)
#define ES_MACRO_VALUE 42
```

## Variables
Variables are just named according to their use. If a variable is global it is suffixed with a `_g`.  
A constant variable will be all uppercase.

Example:
```c
b8_t global_bool_g;
f32_t three_halves = 1.5f;
const f32_t CONSTANT_VARIABLE = 23;
```
