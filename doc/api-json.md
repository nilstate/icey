# json 

### Classes

| Name | Description |
|------|-------------|
| [`Configuration`](#classicy_1_1json_1_1Configuration) | JSON configuration file |
| [`ISerializable`](#classicy_1_1json_1_1ISerializable) |  |

### Members

| Name | Description |
|------|-------------|
| [`JSON_API`](#group__json_1ga1d61ffde86ce1a18fd83194ff0d9a206) |  |

---

#### JSON_API 

```cpp
JSON_API()
```

## Configuration 

> **Extends:** `icy::Configuration`
> **Defined in:** `configuration.h`

JSON configuration file

See base [Configuration](#classicy_1_1json_1_1Configuration) for all accessors

### Members

| Name | Description |
|------|-------------|
| [`Configuration`](#group__json_1gac214c65bca41dbeb2b93de599bf33860) |  |
| [`~Configuration`](#group__json_1ga041eaafa3e57334159495db3f7c8f31a) | Destroys the [Configuration](#classicy_1_1json_1_1Configuration). |
| [`load`](#group__json_1gaa92c45c4c4628a4edb8a3d7652e07750) |  |
| [`load`](#group__json_1ga28c81762c1884c20e06818b09d2ea74a) |  |
| [`save`](#group__json_1ga561f1c272c7e613c15b4827b5be26112) |  |
| [`remove`](#group__json_1gaf79b732c4b7700e0aa93fe276e7e11d7) |  |
| [`removeAll`](#group__json_1gaef6d50b63ce07b1137162050e5181520) |  |
| [`replace`](#group__json_1ga81c6e1be7c048f7df27f45b04fe4c80b) |  |
| [`keys`](#group__json_1gaeb8525cb4022f0c4925e7152c0153af4) |  |
| [`print`](#group__json_1ga248b47860e2324f01700bf297946c5ea) |  |
| [`path`](#group__json_1ga6950af9e99dc2356093aed42c82590c6) |  |
| [`loaded`](#group__json_1gaf4f7ea24a9de424f59d67691aa5c5f06) |  |
| [`getRaw`](#group__json_1ga27463b4236f4ecd5f89e0498f09f2bd5) | If the property with the given key exists, stores the property's value in value and returns true. Otherwise, returns false. |
| [`setRaw`](#group__json_1ga5ac3caa222d6790fbb20813854d75422) | Sets the property with the given key to the given value. An already existing value for the key is overwritten. |
| [`root`](#group__json_1ga01612f32586de58aae915bd75e4108c9) |  |
| [`_loaded`](#group__json_1gabec246e99680f82539c83c0f3a6337ee) |  |
| [`_path`](#group__json_1gafb9956baead97160a9feb7a1e06d8ae5) |  |
| [`_mutex`](#group__json_1ga9d2faff42865fcde1d78319414cc520a) |  |

---

#### Configuration 

```cpp
Configuration()
```

---

#### ~Configuration 

```cpp
virtual ~Configuration()
```

Destroys the [Configuration](#classicy_1_1json_1_1Configuration).

---

#### load 

```cpp
virtual void load(const std::string & path, bool create)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `create` | `bool` |  |

---

#### load 

```cpp
virtual void load(bool create)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `create` | `bool` |  |

---

#### save 

```cpp
virtual void save()
```

---

#### remove 

```cpp
virtual bool remove(const std::string & key)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |

---

#### removeAll 

```cpp
virtual void removeAll(const std::string & baseKey)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `baseKey` | `const std::string &` |  |

---

#### replace 

```cpp
virtual void replace(const std::string & from, const std::string & to)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `from` | `const std::string &` |  |
| `to` | `const std::string &` |  |

---

#### keys 

```cpp
virtual void keys(std::vector< std::string > & keys, const std::string & baseKey)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `keys` | `std::vector< std::string > &` |  |
| `baseKey` | `const std::string &` |  |

---

#### print 

```cpp
virtual void print(std::ostream & ost)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ost` | `std::ostream &` |  |

---

#### path 

```cpp
virtual std::string path()
```

---

#### loaded 

```cpp
virtual bool loaded()
```

---

#### getRaw 

```cpp
virtual bool getRaw(const std::string & key, std::string & value) const
```

If the property with the given key exists, stores the property's value in value and returns true. Otherwise, returns false.

Must be overridden by subclasses.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `std::string &` |  |

---

#### setRaw 

```cpp
virtual void setRaw(const std::string & key, const std::string & value)
```

Sets the property with the given key to the given value. An already existing value for the key is overwritten.

The implementation is responsible for emitting the PropertyChanged signal.

| Parameter | Type | Description |
|-----------|------|-------------|
| `key` | `const std::string &` |  |
| `value` | `const std::string &` |  |

---

#### root 

```cpp
json::value root
```

---

#### _loaded 

```cpp
bool _loaded
```

---

#### _path 

```cpp
std::string _path
```

---

#### _mutex 

```cpp
std::mutex _mutex
```

## ISerializable 

> **Subclasses:** `icy::sched::Scheduler`, `icy::sched::Task`, `icy::sched::Trigger`
> **Defined in:** `iserializable.h`

### Members

| Name | Description |
|------|-------------|
| [`~ISerializable`](#group__json_1ga7e3dfbdaff8b91b43dfb537330faf659) |  |
| [`serialize`](#group__json_1gaaedc895394aa41a56ea12c1640ef3c34) |  |
| [`deserialize`](#group__json_1ga033850497ce6bbae62f3690af1bcb10e) |  |

---

#### ~ISerializable 

```cpp
virtual ~ISerializable() = default
```

---

#### serialize 

```cpp
void serialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

---

#### deserialize 

```cpp
void deserialize(json::value & root)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `root` | `json::value &` |  |

