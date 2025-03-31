#include "ModMain.h"
#include "Prey/GameDll/ark/player/ArkPlayer.h"
#include "Prey/GameDll/ark/player/ArkPlayerComponent.h"
#include "Prey/GameDll/ark/player/ability/ArkAbilityComponent.h"
#include "Prey/CryGame/IGameFramework.h"
#include <Prey/CryGame/Game.h>
#include <Prey/GameDll/ark/ArkListenerManager.h>


ModMain* gMod = nullptr;


//---------------------------------------------------------------------------------
// Mod Initialization
//---------------------------------------------------------------------------------
void ModMain::FillModInfo(ModDllInfoEx& info)
{
    info.modName = "thelivingdiamond.SynapseJunkie";
    info.logTag = "SynapseJunkie";
    info.supportsHotReload = true; // TODO: Add comment/wiki link
}

void ModMain::InitHooks()
{

}

void ModMain::InitSystem(const ModInitInfo& initInfo, ModDllInfo& dllInfo)
{
    BaseClass::InitSystem(initInfo, dllInfo);
    // Your code goes here

}

void ModMain::InitGame(bool isHotReloading)
{
    BaseClass::InitGame(isHotReloading);
    // Your code goes here
    g_pGame->m_pArkListenerManager->RegisterAbilityListener(this);
    gCL->cl->GetFramework()->RegisterListener(this, "SynapseJunkie", EFRAMEWORKLISTENERPRIORITY::FRAMEWORKLISTENERPRIORITY_DEFAULT);


}

//---------------------------------------------------------------------------------
// Mod Shutdown
//---------------------------------------------------------------------------------
void ModMain::ShutdownGame(bool isHotUnloading)
{
    // Unregister the listener
    g_pGame->m_pArkListenerManager->UnregisterAbilityListener(this);
    gCL->cl->GetFramework()->UnregisterListener(this);
    // Your code goes here
    BaseClass::ShutdownGame(isHotUnloading);
}

void ModMain::ShutdownSystem(bool isHotUnloading)
{
    // Your code goes here
    BaseClass::ShutdownSystem(isHotUnloading);
}

//---------------------------------------------------------------------------------
// GUI
//---------------------------------------------------------------------------------
void ModMain::Draw()
{
    // Modders, please move to a method, it's just an example
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Synapse Junkie"))
        {
            ImGui::MenuItem("Addiction Settings");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Synapse Junkie"))
    {
        static bool state = false;
        static time_t loadTime = std::time(nullptr);
        const char* text = !state ? "I love Neuromods" : "FUCK";
        ArkPlayer* player = ArkPlayer::GetInstancePtr();
        int neuromodsUsed = 0;
        if(player != nullptr){
            // this will be the basis for our "mental load" stat
            neuromodsUsed = ArkPlayer::GetInstancePtr()->m_playerComponent.GetAbilityComponent().GetNumNeuromodsUsed();
        }
        ImGui::Text("%s", text);
        ImGui::Text("Load time: %lld", (long long)loadTime);
        ImGui::Text("neuromodsUsed: %d", neuromodsUsed);
        if (ImGui::Button("Take a hit"))
        {
            auto gameTokenSystem = gCL->cl->GetFramework()->GetIGameTokenSystem();
        }


    }

    ImGui::End();
}

//---------------------------------------------------------------------------------
// Main Update Loop
//---------------------------------------------------------------------------------
void ModMain::MainUpdate(unsigned updateFlags)
{
    // Your code goes here
}

void ModMain::OnPostUpdate(float) {

}

// TODO: WHY DOES THIS WORK?????
static char a = 'a';
void ModMain::OnSaveGame(ISaveGame *saveGame) {

    // TODO: WHY MAKE IT FROM a?
    TSerialize *ser = saveGame->AddSection((TSerialize*)&a, "SynapseJunkie");

    int randNum = rand();
    ser->Value("randomNumber", randNum);
    CryLog("Saved random number: {}", randNum);
}

void ModMain::OnLoadGame(ILoadGame *loadGame) {
    if(loadGame != nullptr){
        if(!loadGame->HaveSection("SynapseJunkie")){
            return;
        }

        // TODO: WHY MAKE IT FROM a? WHY DOES THIS WORK????
        std::unique_ptr<TSerialize> serPtr = std::make_unique<TSerialize>((ISerialize*)&a);
        auto b = loadGame->GetSection(&serPtr, "SynapseJunkie");
        if(b == nullptr){
            return;
        }
        TSerialize *ser = b->get();
        int randNum = 0;
        ser->Value("randomNumber", randNum);
        CryLog("Loaded random number: {}", randNum);

    }
}

void ModMain::OnLevelEnd(const char *someString) {

}

void ModMain::OnActionEvent(const SActionEvent &event) {

}

void ModMain::OnPreRender() {
}

void ModMain::OnSavegameFileLoadedInMemory(const char *something) {
}

void ModMain::OnForceLoadingWithFlash() {
}

void ModMain::OnAbilityAdded(uint64_t _abilityID) {
    CryLog("Ability added: %llu", _abilityID);
}

void ModMain::OnBecomeAlien() {
    CryLog("Became alien");
}

//---------------------------------------------------------------------------------
// Mod Interfacing
//---------------------------------------------------------------------------------
#ifdef EXAMPLE
void *ModMain::QueryInterface(const char *ifaceName)
{
    // This function is called by other mods to get interfaces.
    // Check if you implement the interface. If so, return a pointer to it.
    // This example assumes ModMain inherits from the interface class. You may choose a different object if you want.
    if (!strcmp(ifaceName, IModName::IFACE_NAME))
        return static_cast<IModName*>(this);

    return nullptr;
}

void ModMain::Connect(const std::vector<IChairloaderMod *> &mods)
{
    IModName* pModName = nullptr; // Add this to ModMain

    // You can use one of these helper functions. They will handle finding and version checking.
    pModName = GetInterfaceFromModList<IModName>(mods, "Other Mod Name");
    pModName = GetRequiredInterfaceFromModList<IModName>(mods, "Other Mod Name");
}
#endif

//---------------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------------
extern "C" DLL_EXPORT IChairloaderMod* ClMod_Initialize()
{
    CRY_ASSERT(!gMod);
    gMod = new ModMain();
    return gMod;
}

extern "C" DLL_EXPORT void ClMod_Shutdown()
{
    CRY_ASSERT(gMod);
    delete gMod;
    gMod = nullptr;
}

// Validate that declarations haven't changed
static_assert(std::is_same_v<decltype(ClMod_Initialize), IChairloaderMod::ProcInitialize>);
static_assert(std::is_same_v<decltype(ClMod_Shutdown), IChairloaderMod::ProcShutdown>);
