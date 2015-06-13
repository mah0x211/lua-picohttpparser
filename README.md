# lua-picohttpparser

picohttpparser module for lua.

## Dependencies

- luarocks-fetch-gitrec: https://github.com/siffiejoe/luarocks-fetch-gitrec
- picohttpparser: https://github.com/h2o/picohttpparser


## Installation

```sh
luarocks install picohttpparser --from=http://mah0x211.github.io/rocks/
```


## Create new picohttpparser object

### phr, err = picohttpparser.new( [maxheader:number] )

**Parameters**

- `maxheader:number`: maximum number of headers. `default 20`


**Returns**

1. `phr:userdata`: picohttpparser object on success. or, a nil on failure.
2. `err:string`: error message.


## Methods

### req, consume = phr:parseRequest( str:string [, prevlen:number] )

parse HTTP request.

**Parameters**

- `str:string`: HTTP request string.
- `prevlen:number`: previous str length. `default 0`


**Returns**

1. `req:table`: request table on success, or a nil on failure.
2. `consume:number`: number of bytes consumed on success, -1 on invalid request, -2 on request is incomplete.


**Field names of request table**

- `method:string`: method name.
- `path:string`: pathname.
- `minor_version:number`: minor version of HTTP version.
- `header:table`: request headers.


### res, consume = phr:parseResponse( str:string [, prevlen:number] )

parse HTTP response.

**Parameters**

- `str:string`: HTTP request string.
- `prevlen:number`: previous str length. `default 0`


**Returns**

1. `res:table`: response table on success, or a nil on failure.
2. `consume:number`: number of bytes consumed on success, -1 on invalid request, -2 on request is incomplete.


**Field names of response table**

- `minor_version:number`: minor version of HTTP version.
- `status:number`: status code.
- `message:string`: message string.
- `header:table`: response headers.


## Usage

```lua
local reqstr = 
    "GET /wp-content/uploa<ds/2010/03/hello-kitty-darth-vader-pink.jpg HTTP/1.1\r\n" ..
    "Host: www.kittyhell.com\r\n" ..
    "User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.6; ja-JP-mac; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3 Pathtraq/0.9\r\n" ..
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" ..
    "Accept-Language: ja,en-us;q=0.7,en;q=0.3\r\n" ..
    "Accept-Encoding: gzip,deflate\r\n" ..
    "Accept-Charset: Shift_JIS,utf-8;q=0.7,*;q=0.7\r\n" ..
    "Keep-Alive: 115\r\n" ..
    "Connection: keep-alive\r\n" ..
    "Cookie: wp_ozh_wsa_visits=2; wp_ozh_wsa_visit_lasttime=xxxxxxxxxx; " ..
    "__utma=xxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.xxxxxxxxxx.x; " ..
    "__utmz=xxxxxxxxx.xxxxxxxxxx.x.x.utmccn=(referral)|utmcsr=reader.livedoor.com|utmcct=/reader/|utmcmd=referral\r\n" ..
    "\r\n";
local phr = require('picohttpparser').new();
local req, consume = phr:parseRequest( buf );
```

