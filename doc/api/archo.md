# archo 

### Classes

| Name | Description |
|------|-------------|
| [`ZipFile`](#structicy_1_1archo_1_1ZipFile) |  |
| [`FileInfo`](#structicy_1_1archo_1_1ZipFile_1_1FileInfo) |  |

### Members

| Name | Description |
|------|-------------|
| [`Archo_API`](#group__archo_1ga804678b23898d4f64d5686ef544ccfe2) | Shared library exports. |

---

#### Archo_API 

```cpp
Archo_API()
```

Shared library exports.

## ZipFile 

> **Defined in:** `zipfile.h`

### Members

| Name | Description |
|------|-------------|
| [`ZipFile`](#group__archo_1gafbd603f7e87970b25761dde6ee43ed53) |  |
| [`ZipFile`](#group__archo_1ga83472457ecd60b679786812154a48b2f) |  |
| [`~ZipFile`](#group__archo_1ga3526297ee66287bd3392b5496e29e296) |  |
| [`open`](#group__archo_1ga4505de4a4b0258c1be3640370790b3e5) |  |
| [`opened`](#group__archo_1ga2cb3083919bf19fdc8440681095ad52b) |  |
| [`close`](#group__archo_1gad5f16432c7097c4f0f3376ec06d9b8aa) |  |
| [`extract`](#group__archo_1ga729b7bfd554bf838c855ad42b9e4039d) | Extracts the archive contents to the given directory path. |
| [`extractCurrentFile`](#group__archo_1gac5f1b12cfb628a7fba60abc3fca6e9dc) |  |
| [`goToFirstFile`](#group__archo_1gaa70485384e6dbcc5783b3228e9b9f942) |  |
| [`goToNextFile`](#group__archo_1gaa197ae9f0e8e670d365f0cb5e4f2cb94) |  |
| [`openCurrentFile`](#group__archo_1ga084ce7163c3a21c4bb905dab42aee7ba) |  |
| [`closeCurrentFile`](#group__archo_1gaef2d3fd19db8f1d24987e949b413b927) |  |
| [`currentFileName`](#group__archo_1ga4ed5731ad68cd4a8be42342963f245ac) |  |
| [`info`](#group__archo_1ga031d3193c306c10b0092e5815aead8cb) |  |
| [`fp`](#group__archo_1gaf810f2266baaef1a7c693a1a5df6768e) |  |

---

#### ZipFile 

```cpp
ZipFile()
```

---

#### ZipFile 

```cpp
ZipFile(const std::string & file)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | `const std::string &` |  |

---

#### ~ZipFile 

```cpp
~ZipFile()
```

---

#### open 

```cpp
void open(const std::string & file)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | `const std::string &` |  |

---

#### opened 

```cpp
bool opened() const
```

---

#### close 

```cpp
void close()
```

---

#### extract 

```cpp
void extract(const std::string & path)
```

Extracts the archive contents to the given directory path.

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |

---

#### extractCurrentFile 

```cpp
bool extractCurrentFile(const std::string & path, bool whiny)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `path` | `const std::string &` |  |
| `whiny` | `bool` |  |

---

#### goToFirstFile 

```cpp
bool goToFirstFile()
```

---

#### goToNextFile 

```cpp
bool goToNextFile()
```

---

#### openCurrentFile 

```cpp
void openCurrentFile()
```

---

#### closeCurrentFile 

```cpp
void closeCurrentFile()
```

---

#### currentFileName 

```cpp
std::string currentFileName()
```

---

#### info 

```cpp
std::vector< FileInfo > info
```

---

#### fp 

```cpp
unzFile fp
```

## FileInfo 

> **Defined in:** `zipfile.h`

### Members

| Name | Description |
|------|-------------|
| [`path`](#group__archo_1gae91e44aaf9e8629c002ed91ac4e3f8d8) |  |
| [`compressedSize`](#group__archo_1gaef7fe29bc86babd96a66766fd07b2733) |  |
| [`uncompressedSize`](#group__archo_1ga35738340dd3e5e207c70d700a883abba) |  |

---

#### path 

```cpp
std::string path
```

---

#### compressedSize 

```cpp
size_t compressedSize
```

---

#### uncompressedSize 

```cpp
size_t uncompressedSize
```

## FileInfo 

> **Defined in:** `zipfile.h`

### Members

| Name | Description |
|------|-------------|
| [`path`](#group__archo_1gae91e44aaf9e8629c002ed91ac4e3f8d8) |  |
| [`compressedSize`](#group__archo_1gaef7fe29bc86babd96a66766fd07b2733) |  |
| [`uncompressedSize`](#group__archo_1ga35738340dd3e5e207c70d700a883abba) |  |

---

#### path 

```cpp
std::string path
```

---

#### compressedSize 

```cpp
size_t compressedSize
```

---

#### uncompressedSize 

```cpp
size_t uncompressedSize
```

