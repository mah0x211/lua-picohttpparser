package = "picohttpparser"
version = "scm-1"
source = {
    url = "git://github.com/mah0x211/lua-picohttpparser.git"
}
description = {
    summary = "picohttpparser for lua",
    homepage = "https://github.com/mah0x211/lua-picohttpparser",
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1"
}
build = {
    type = "command",
    build_command = [[
        git submodule update && autoreconf -ivf && CFLAGS="$(CFLAGS)" CPPFLAGS="-I$(LUA_INCDIR)" LIBFLAG="$(LIBFLAG)" OBJ_EXTENSION="$(OBJ_EXTENSION)" LIB_EXTENSION="$(LIB_EXTENSION)" LIBDIR="$(LIBDIR)" ./configure && make clean && make
    ]],
    install_command = "make install"
}


