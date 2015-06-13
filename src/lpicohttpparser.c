/*
 *  Copyright 2015 Masatoshi Teruya. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a 
 *  copy of this software and associated documentation files (the "Software"), 
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 *
 *  lpicohttpparser.c
 *  lua-picohttpparser
 *
 *  Created by Masatoshi Teruya on 15/06/13.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <lua.h>
#include <lauxlib.h>
#include "picohttpparser.h"


#define MODULE_MT   "picohttpparser"

#define DEFAULT_MAX_HDR     20

// helper macros
#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_num2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushnumber(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_strl2tbl(L,k,v,vl) do{ \
    lua_pushstring(L,k); \
    lua_pushlstring(L,v,vl); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_strll2tbl(L,k,kl,v,vl) do{ \
    lua_pushlstring(L,k,kl); \
    lua_pushlstring(L,v,vl); \
    lua_rawset(L,-3); \
}while(0)


typedef struct {
    struct phr_header *headers;
    uint8_t maxhdr;
} lpicohttpparser_t;


static int parse_request_lua( lua_State *L )
{
    lpicohttpparser_t *p = luaL_checkudata( L, 1, MODULE_MT );
    size_t len = 0;
    const char *buf = luaL_checklstring( L, 2, &len );
    lua_Integer prevlen = luaL_optint( L, 3, 0 );
    size_t nhdr = p->maxhdr;
    const char *method = NULL;
    size_t mlen = 0;
    const char *path = NULL;
    size_t plen = 0;
    int minor_ver = 0;
    size_t i = 0;
    
    
    // returns number of bytes consumed if successful, 
    // -2 if request is partial,
    // -1 if failed
    prevlen = phr_parse_request( buf, len, &method, &mlen, &path, &plen,
                                 &minor_ver, p->headers, &nhdr, 
                                 (size_t)prevlen );
    switch( prevlen )
    {
        // invalid request
        case -1:
        // request is incomplete
        case -2:
            lua_pushnil( L );
        break;
        
        // successfully parsed the request
        default:
            lua_createtable( L, 0, 4 );
            lstate_strl2tbl( L, "method", method, mlen );
            lstate_strl2tbl( L, "path", path, plen );
            lstate_num2tbl( L, "minor_version", minor_ver );
            // create header table
            lua_pushstring( L, "header" );
            lua_createtable( L, 0, nhdr );
            for(; i < nhdr; i++ ){
                lstate_strll2tbl( L, p->headers[i].name, p->headers[i].name_len,
                                  p->headers[i].value, p->headers[i].value_len );
            }
            lua_rawset( L, -3 );
    }
    
    lua_pushinteger( L, prevlen );
    return 2;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, MODULE_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    free( ((lpicohttpparser_t*)lua_touserdata( L, 1 ))->headers );
    return 0;
}


static int new_lua( lua_State *L )
{
    int maxhdr = luaL_optint( L, 1, DEFAULT_MAX_HDR );
    lpicohttpparser_t *p = NULL;
    
    if( maxhdr > UINT8_MAX ){
        return luaL_argerror( L, 1, "maxhdr must be less than UINT8_MAX" );
    }
    else if( ( p = lua_newuserdata( L, sizeof( lpicohttpparser_t ) ) ) &&
             ( p->headers = calloc( maxhdr, sizeof( struct phr_header ) ) ) ){
        p->maxhdr = maxhdr;
        luaL_getmetatable( L, MODULE_MT );
        lua_setmetatable( L, -2 );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


LUALIB_API int luaopen_picohttpparser( lua_State *L )
{
    struct luaL_Reg mmethods[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg methods[] = {
        // method
        { "parseRequest", parse_request_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethods;
    
    // create metatable
    luaL_newmetatable( L, MODULE_MT );
    // metamethods
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    // methods
    ptr = methods;
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    lua_rawset( L, -3 );
    // remove metatable from stack
    lua_pop( L, 1 );
    
    
    // register allocator
    lua_createtable( L, 0, 1 );
    lstate_fn2tbl( L, "new", new_lua );

    return 1;
}



