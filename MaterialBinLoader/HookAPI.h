#pragma once

#include <Hash.h>
#include <string>
#include <Windows.h>
#include <Psapi.h>
#include "Memory.h"

#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))

#include <detours/detours.h>
#include <unordered_map>

namespace Utils {
    static LPCSTR hModuleName;
    static bool running;
}

inline static void HookFunction__begin() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
}
inline static long HookFunction__finalize() {
    return DetourTransactionCommit();
}
static inline int realHook(void* oldfunc, void** poutold, void* newfunc) {
    void* target = oldfunc;
    HookFunction__begin();
    int rv = DetourAttach(&target, newfunc);
    HookFunction__finalize();
    *poutold = target;
    return rv;
}

inline int HookFunction(void* oldfunc, void** poutold, void* newfunc) {
    static std::unordered_map<void*, void**> ptr_pori;
    auto it = ptr_pori.find(oldfunc);
    if (it == ptr_pori.end()) {
        int rv = realHook(oldfunc, poutold, newfunc);
        if (rv != 0)
            return rv;
        ptr_pori[oldfunc] = poutold;
        return 0;
    }
    else {
        *poutold = *it->second;
        *it->second = newfunc;
        return 0;
    }
}

template <typename RTN = void, typename... Args>
RTN inline VirtualCall(void const* _this, uintptr_t off, Args... args) {
    return (*(RTN(**)(void const*, Args...))(*(uintptr_t*)_this + off))(_this, args...);
}

template <typename T, int off>
inline T& dAccess(void* ptr) {
    return *(T*)(((uintptr_t)ptr) + off);
}
template <typename T, int off>
inline T const& dAccess(void const* ptr) {
    return *(T*)(((uintptr_t)ptr) + off);
}
template <typename T>
inline T& dAccess(void* ptr, uintptr_t off) {
    return *(T*)(((uintptr_t)ptr) + off);
}
template <typename T>
inline const T& dAccess(void const* ptr, uintptr_t off) {
    return *(T*)(((uintptr_t)ptr) + off);
}
inline uintptr_t FindSig(const char* szSignature) {
    const char* pattern = szSignature;
    uintptr_t firstMatch = 0;
    static const uintptr_t rangeStart = (uintptr_t)GetModuleHandleA(Utils::hModuleName);
    static MODULEINFO miModInfo;
    static bool init = false;
    if (!init) {
        init = true;
        GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
    }
    static const uintptr_t rangeEnd = rangeStart + miModInfo.SizeOfImage;
    BYTE patByte = GET_BYTE(pattern);
    const char* oldPat = pattern;

    for (uintptr_t pCur = rangeStart; pCur < rangeEnd; pCur++) {
        if (!*pattern)
            return firstMatch;

        while (*(PBYTE)pattern == ' ')
            pattern++;

        if (!*pattern)
            return firstMatch;

        if (oldPat != pattern) {
            oldPat = pattern;
            if (*(PBYTE)pattern != '\?')
                patByte = GET_BYTE(pattern);
        }
        if (*(PBYTE)pattern == '\?' || *(BYTE*)pCur == patByte) {
            if (!firstMatch)
                firstMatch = pCur;

            if (!pattern[2] || !pattern[1])
                return firstMatch;
            pattern += 2;
        }
        else {
            pattern = szSignature;
            firstMatch = 0;
        }
    }
    return 0;
}

template <size_t N>
struct FixedString {
    char buf[N + 1]{};
    constexpr FixedString(char const* s) {
        for (unsigned i = 0; i != N; ++i)
            buf[i] = s[i];
    }
    constexpr operator char const* () const {
        return buf;
    }
};
template <size_t N>
FixedString(char const (&)[N])->FixedString<N - 1>;

template <FixedString Fn>
__declspec(selectany) void* __dlsym_ptr_cache = dlsym_real(Fn);

#define VA_EXPAND(...) __VA_ARGS__
template <FixedString Fn, typename ret, typename... p>
static inline auto __imp_Call() {
    return ((ret(*)(p...))((void*)FindSig(Fn)));
}

#define AddrCall(fn, ret, ...) (__imp_Call<fn, ret, __VA_ARGS__>())

class THookRegister {
public:
    THookRegister(uintptr_t address, void* hook, void** org) {
        auto ret = HookFunction((void*)address, org, hook);
        if (ret != 0) {
            printf("FailedToHook: %lld\n", address);
        }
    }
    template <typename T>
    THookRegister(uintptr_t address, T hook, void** org) {
        union {
            T a;
            void* b;
        } hookUnion;
        hookUnion.a = hook;
        THookRegister(address, hookUnion.b, org);
    }
};

#define VA_EXPAND(...) __VA_ARGS__
template <CHash, CHash>
struct THookTemplate;
template <CHash, CHash>
extern THookRegister THookRegisterTemplate;

#define _AInstanceHook(class_inh, pclass, iname, addr, ret, ...)                              \
    template <>                                                                              \
    struct THookTemplate<do_hash(iname), do_hash2(iname)> class_inh {                        \
        typedef ret (THookTemplate::*original_type)(__VA_ARGS__);                            \
        static original_type& _original() {                                                  \
            static original_type storage;                                                    \
            return storage;                                                                  \
        }                                                                                    \
        template <typename... Params>                                                        \
        static ret original(pclass* _this, Params&&... params) {                             \
            return (((THookTemplate*)_this)->*_original())(std::forward<Params>(params)...); \
        }                                                                                    \
        ret _hook(__VA_ARGS__);                                                              \
    };                                                                                       \
    template <>                                                                              \
    static THookRegister THookRegisterTemplate<do_hash(iname), do_hash2(iname)>{             \
        addr, &THookTemplate<do_hash(iname), do_hash2(iname)>::_hook,                         \
        (void**)&THookTemplate<do_hash(iname), do_hash2(iname)>::_original()};               \
    ret THookTemplate<do_hash(iname), do_hash2(iname)>::_hook(__VA_ARGS__)

#define _AInstanceDefHook(iname, addr, ret, type, ...) \
    _AInstanceHook(                                   \
        : public type, type, iname, addr, ret, VA_EXPAND(__VA_ARGS__))
#define _AInstanceNoDefHook(iname, addr, ret, ...) \
    _AInstanceHook(, void, iname, addr, ret, VA_EXPAND(__VA_ARGS__))

#define _AStaticHook(pclass, iname, addr, ret, ...)                               \
    template <>                                                                  \
    struct THookTemplate<do_hash(iname), do_hash2(iname)> pclass {               \
        typedef ret (*original_type)(__VA_ARGS__);                               \
        static original_type& _original() {                                      \
            static original_type storage;                                        \
            return storage;                                                      \
        }                                                                        \
        template <typename... Params>                                            \
        static ret original(Params&&... params) {                                \
            return _original()(std::forward<Params>(params)...);                 \
        }                                                                        \
        static ret _hook(__VA_ARGS__);                                           \
    };                                                                           \
    template <>                                                                  \
    static THookRegister THookRegisterTemplate<do_hash(iname), do_hash2(iname)>{ \
        addr, &THookTemplate<do_hash(iname), do_hash2(iname)>::_hook,             \
        (void**)&THookTemplate<do_hash(iname), do_hash2(iname)>::_original()};   \
    ret THookTemplate<do_hash(iname), do_hash2(iname)>::_hook(__VA_ARGS__)

#define _AStaticDefHook(iname, addr, ret, type, ...) \
    _AStaticHook(                                   \
        : public type, iname, addr, ret, VA_EXPAND(__VA_ARGS__))
#define _AStaticNoDefHook(iname, addr, ret, ...) \
    _AStaticHook(, iname, addr, ret, VA_EXPAND(__VA_ARGS__))


#define AHook2(iname, ret, addr, ...) _AStaticNoDefHook(iname, addr, ret, VA_EXPAND(__VA_ARGS__))
#define AHook(iname, ret, sym, ...) _AStaticNoDefHook(iname, FindSig(sym), ret, VA_EXPAND(__VA_ARGS__))

#define AStaticHook2(iname, ret, addr, type, ...) \
    _AStaticDefHook(iname, addr, ret, type, VA_EXPAND(__VA_ARGS__))
#define AStaticHook(iname, ret, sym, type, ...) \
    _AStaticDefHook(iname, FindSig(sym), ret, type, VA_EXPAND(__VA_ARGS__))

#define AClasslessInstanceHook2(iname, ret, addr, ...) \
    _AInstanceNoDefHook(iname, sym, ret, VA_EXPAND(__VA_ARGS__))
#define AClasslessInstanceHook(iname, ret, sym, ...) \
    _AInstanceNoDefHook(iname, FindSig(sym), ret, VA_EXPAND(__VA_ARGS__))

#define AInstanceHook2(iname, ret, addr, type, ...) \
    _AInstanceDefHook(iname, addr, ret, type, VA_EXPAND(__VA_ARGS__))
#define AInstanceHook(iname, ret, sym, type, ...) \
    _AInstanceDefHook(iname, FindSig(sym), ret, type, VA_EXPAND(__VA_ARGS__))
