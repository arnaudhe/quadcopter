# Overview

## Topology

* Not connected
* Communication 1-to-1. No concurrent sessions.
* Designed to rely on TCP/IP or RF
* 2 modes
    * JSON 
    * Binary
* 3 kinds of commands
    * Read
    * Write
    * Notify
* Roles of client / server
    * server = ressources supplier
    * client = ressouces consumer

## Read transaction

Request/Response transaction
```
 CLIENT                             SERVER
   _                                   _
   |     READ(seq, list of values)     |
   |---------------------------------->|
   |                                   |
   |     READ(seq, list of status)     |
   |<----------------------------------|
   |                                   |
```

## Write transaction

Request/Response transaction
```
 CLIENT                             SERVER
   _                                   _
   |     WRITE(seq, list of values)    |
   |---------------------------------->|
   |                                   |
   |     WRITE(seq, list of status)    |
   |<----------------------------------|
   |                                   |
```

## Notify transaction

Response-only transaction
```
 CLIENT                             SERVER
   _                                   _
   |                                   |
   |    NOTIFY(seq, list of status)    |
   |<----------------------------------|
   |                                   |
```

## Status

| status | code |
|---|---|
| success | 0 |
| not found | 1 |
| bad type | 2 |
| bad value | 3 |
| bad permissions | 4 |

# Binary mode 

## Global frame structure

```
+---------+------+--------+---------+-----+------+--------+---------+
| CONTROL | ID 1 | TYPE 1 | VALUE 1 | ... | ID N | TYPE N | VALUE N |
+---------+------+--------+---------+-----+------+--------+---------+
```

## Control Bytes

```
+-------+-------------------------------+-------------------------------+
| BYTES |              MSB              |              LSB              |
+-------+-------------------------------+-------------------------------+
|  BIT  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
+-------+---------------+---------------+-------------------------------+
| FIELD |  COMMAND  |DIR|      NUM      |         SEQUENCE NUMBER       |
+-------+---------------+---------------+-------------------------------+
```

### Command

| value | command |
|---|---|
| 1 | read |
| 2 | write |
| 3 | notify |

### Direction

| value | command |
|---|---|
| 0 | request |
| 1 | response |

### Num

Number of ressources (id, value or status) read/written/notified

### Sequence number

Auto-incremented unsigned integer (0 to 255) to identify a transaction

## ID

 * The first byte is the depth of the node to address
 * The following bytes indicates the recursive tree nodes' id.

```
+-----+---------+---------+-------+---------+
| LEN | LEVEL 1 | LEVEL 2 |  ...  | LEVEL N |
+-----+---------+---------+-------+---------+
```

where "level x" is the id of the json node in the tree

## Type

Indicates the length of the following value

| type | code | length |
|---|---|---|
| `empty` | 0 | 0 |
| `integer` | 1 | 4 |
| `float` | 2 | 4 |
| `enum` | 3 | 1 |
| `bool` | 4 | 1 |
| `status` | 5 | 1 |

`empty` type is used on READ requests

## Value

Raw encoded value, in little endian.

# JSON mode

## Syntax
The two following syntaxes are supported :

```
{
    "command" : "read/write/notify",
    "response" : "request/response",
    "sequence_number" : 0-255,
    "ressources/status" : {
        "level1-1" : {
            "level2-1" : value,
            "level2-2" : value
        },
        "level1-2" : {
            "level2-1" : {
                level3-1" : value
            }
        }
    }
}
```

```
{
    "command" : "read/write/notify",
    "response" : "request/response",
    "sequence_number" : 0-255,
    "ressources/status" : {
        "level1-1.level2-1" : value,
        "level1-1.level2-2" : value,
        "level1-2.level2-1.level3-1" : value
    }
}
```

For read requests 
```
{
    "command" : "read",
    "response" : "request",
    "sequence_number" : 0-255,
    "ressources" : [
        "level1-1.level2-1",
        "level1-1.level2-2",
        "level1-2.level2-1.level3-1"
    ]
}
```

## Note

For `enum` or `status`, either string value or integer value can be used.