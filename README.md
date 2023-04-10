## SJSON: a JSON library that performs as fast as JSON.parse()

[Here](https://shree-c.github.io/p_blog/articles/json_parser_part_1/) is the detailed blog post of how I built it(with screenshots).

## cool facts

- no memory leaks
- no need to free memory: smart pointers are used
- performs as fast\[or faster when <10MB\] as JSON.parse up to 10MB size of string
- pretty printing

## How to use

- clone the repository inside a folder
- include header files: `serializer.h print.h`
- printer.cpp and serializer.cpp should be compiled and linked with your binary

## API

- all entities are defined in SJSON namespace

### serialization: serializer.h

- create a new Serializer object with JSON string

```CPP
std::string json_string{"{\"bar\": 10 }"}

SJSON::Serializer foo(json_string);

SJSON::Json_entity_shared_ptr foo_ptr = foo.serialize();
```

- `Json_entity_shared_ptr` has `get_type()` method. It returns `SJSON::Data_type`
- definition of `SJSON::Data_type`

```CPP
enum class Data_type { OBJECT, BOOLEAN, NUMBER, ARRAY, TNULL, STRING };
```

- cast the object to its respective type based on the `Date\_type` returned by `get_type()`

| Data_type | to be casted to          |
| --------- | ------------------------ |
| OBJECT    | `Json_obj_shared_ptr`    |
| BOOLEAN   | `Json_bool_shared_ptr`   |
| NUMBER    | `Json_number_shared_ptr` |
| ARRAY     | `Json_arr_shared_ptr`    |
| TNULL     | `Json_null_shared_ptr`   |
| STRING    | `Json_shared_shared_ptr` |

- if the `Data_type` is ARRAY, `get_value()` returns `std::vector<Json_entity_shared_ptr>`
- if the `Data_type` is Object, `get_value()` returns `std::map<std::string, Json_entity_shared_ptr>`

## printing : print.h

- Use `SJSON::print_json(Json_entity_shared_ptr)` to pretty print JSON.

checkout `Makefile` to replicate tests
