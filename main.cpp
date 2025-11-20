#include "includes.h"
#include "game_tick.h"

uintptr_t           uworld_addr;
uintptr_t           post_render_addr;
settings_manager    settings;

__int64 __fastcall post_render_func_hook(uintptr_t p_this, UCanvas* canvas)
{
    typedef __int64(__fastcall* original_render_typedef)(uintptr_t p_this, UCanvas* canvas);
    original_render_typedef original_post_render = original_render_typedef(post_render_addr);

    /* why trampoline hook when we can always just do this? =p */
    hook::disable((void*)post_render_addr);
    __int64 r = original_post_render(p_this, canvas);
    hook::enable((void*)post_render_addr);

    __try
    {
    game_tick(settings, canvas, uworld_addr);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    return r;
}

void init()
{
    uintptr_t module_base = (uintptr_t)GetModuleHandleW(L"ShooterGame.exe");

    /* FEngineLoop::PreInit has a refrence to uobjects */
    uintptr_t uobjects_ref = pattern_scan(module_base, "8B 05 ? ? ? ? 40 38 ? ? ? 0F 45");

    if (!uobjects_ref)
        return;

    /* 8B 05 66 5d 85 04    mov    eax,[addr] */
    signed int uobjects_offset = *(DWORD*)(uobjects_ref + 0x2);
    uintptr_t uobjects = (uintptr_t)((uobjects_ref + 0x6) + uobjects_offset);


    /* FName::GetNames has a refrence to fNames */
    uintptr_t fnames_ref = pattern_scan(module_base, "48 8B ? ? ? ? ? 48 85 ? 0F 85 ? ? ? ? 38 05 ? ? ? ? 48 89 ? ? ? 74 ? FF 15 ? ? ? ? 3B 05 ? ? ? ? 74 ? FF 15 ? ? ? ? 3B 05 ? ? ? ? 74 ? 4C 8D ? ? ? ? ? 48 8D ? ? ? ? ? 48 8D ? ? ? ? ? 41 B8 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 48 8B ? 48 85 ? 74 ? 33 C0");

    if (!fnames_ref)
        return;

    /* 48 8B 05 fd 21 8a 02    mov    rax,[addr] */
    signed int fnames_offset = *(DWORD*)(fnames_ref + 0x3);
    uintptr_t fnames = (uintptr_t)((fnames_ref + 0x7) + fnames_offset);


    /* uGameEngine::Tick has a refrence to uworld */
    uintptr_t uworld_ref = pattern_scan(module_base, "48 8B ? ? ? ? ? 48 39 ? ? ? ? ? 74 ? FF C3");

    if (!uworld_ref)
        return;

    /* 48 8B 05 b8 98 05 02    mov    rax,[addr] */
    signed int uworld_offset = *(DWORD*)(uworld_ref + 0x3);
    uintptr_t uworld = (uintptr_t)((uworld_ref + 0x7) + uworld_offset);

    /* If uobjects, fnames, or uworld are wrong , we will now crash */

    uworld_addr       = uworld;
    UObject::GObjects = (TUObjectArray*)uobjects;
    FName::GNames     = *(TNameEntryArray**)fnames;

    /* The ue4 game canvas doesn't allow drawing in every state. Since we want to call render functions, we can hook post-render,
    as it's a state that does allow drawing, and passes us in the Canvas object directly. */
    post_render_addr = pattern_scan(module_base, "48 89 ? ? ? 57 48 83 EC ? F6 81 84 00 00 00");

    if (post_render_addr)
        hook::make_hook((void*)post_render_addr, post_render_func_hook);

    uintptr_t be_1 = pattern_scan(module_base, "48 89 ? ? ? 57 48 83 EC ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 33 D2");
    uintptr_t be_2 = pattern_scan(module_base, "48 89 ? ? ? 55 48 8D ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 48 8B ? 48 85");

    if(be_1)
        prot_memcpy((void*)be_1, (BYTE*)"\xC3", 1);
    if(be_2)
        prot_memcpy((void*)be_2, (BYTE*)"\xC3", 1);

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        init();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
