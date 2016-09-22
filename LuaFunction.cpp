#include "LuaFunction.hpp"

LuaRef::LuaRef(): L(nullptr), ref_(LUA_NOREF)
{
}

LuaRef::LuaRef(lua_State* aL, int index) : L(aL), ref_(LUA_NOREF)
{
    lua_pushvalue(L, index);
    ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
}

LuaRef::~LuaRef()
{
    unref();
}

LuaRef::LuaRef(const LuaRef& other): L(nullptr), ref_(LUA_NOREF)
{
    *this = other;
}

LuaRef& LuaRef::operator=(const LuaRef& rhs)
{
    if (this != &rhs)
    {
        rhs.push();
        reset(rhs.L, -1);
        lua_pop(L, 1);
    }
    return *this;
}

LuaRef::LuaRef(LuaRef&& other): L(nullptr), ref_(LUA_NOREF)
{
    *this = std::move(other);
}

LuaRef& LuaRef::operator=(LuaRef&& rhs)
{
    if (this != &rhs)
    {
        unref();

        L = rhs.L;
        ref_ = rhs.ref_;

        rhs.L = nullptr;
        rhs.ref_ = LUA_NOREF;
    }
    return *this;
}

LuaRef::operator bool() const
{
    return ref_ != LUA_NOREF;
}

void LuaRef::reset(lua_State* aL, int index)
{
    unref();
    if (aL != nullptr) {
        L = aL;
        lua_pushvalue(L, index);
        ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
    }
}

void LuaRef::push() const
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref_);
}

lua_State* LuaRef::state() const
{
    return L;
}

void LuaRef::unref() const
{
    if (L && ref_ != LUA_NOREF && ref_ != LUA_REFNIL)
        luaL_unref(L, LUA_REGISTRYINDEX, ref_);
}


LuaFunction::LuaFunction(): LuaRef(), trackback_(0),returnCnt_(0)
{
}

LuaFunction::LuaFunction(lua_State* aL, int index) : LuaRef(aL, index), trackback_(0),returnCnt_(0)
{
    luaL_checktype(aL, index, LUA_TFUNCTION);
}

LuaFunction::LuaFunction(const LuaFunction& other): LuaRef()
{
    *this = other;
}

LuaFunction& LuaFunction::operator=(const LuaFunction& rhs)
{
    if (this != &rhs)
    {
        returnCnt_ = rhs.returnCnt_;
        checkReturn_ = rhs.checkReturn_;
        rhs.push();
        luaL_checktype(rhs.L, -1, LUA_TFUNCTION);
        reset(rhs.L, -1);
        lua_pop(L, 1);
    }
    return *this;
}

LuaFunction::LuaFunction(LuaFunction&& other): LuaRef()
{
    *this = std::move(other);
}

LuaFunction& LuaFunction::operator=(LuaFunction&& rhs)
{
    if (this != &rhs)
    {
        unref();

        L = rhs.L;
        ref_ = rhs.ref_;
        returnCnt_ = rhs.returnCnt_;
        checkReturn_ = rhs.checkReturn_;

        rhs.L = nullptr;
        rhs.ref_ = LUA_NOREF;
        rhs.checkReturn_ = nullptr;
    }
    return *this;
}

void LuaFunction::operator()() const
{
    ppush();
    pcall();
}

void LuaFunction::ppush() const
{
    lua_getglobal(L, "__G__TRACKBACK__");
    trackback_ = lua_gettop(L);
    push();
    luaL_checktype(L, -1, LUA_TFUNCTION);
}

void LuaFunction::pcall() const
{
    int argc = lua_gettop(L) - trackback_ - 1;
    lua_pcall(L, argc, returnCnt_, trackback_);
    if (checkReturn_)
      checkReturn_(L);
    lua_settop(L, trackback_ - 1); // remove trackback and any thing above it.
}

void LuaFunction::pusharg(bool v) const
{
    lua_pushboolean(L, v);
}

void LuaFunction::pusharg(float v) const
{
    lua_pushnumber(L, v);
}

void LuaFunction::pusharg(double v) const
{
    lua_pushnumber(L, v);
}

void LuaFunction::pusharg(int v) const
{
    lua_pushinteger(L, v);
}

void LuaFunction::pusharg(const std::string& v) const
{
    lua_pushlstring(L, v.data(), v.size());
}

void LuaFunction::pusharg(const char* v) const
{
    lua_pushstring(L, v);
}
