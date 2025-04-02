#include "ModMain.h"
#include "Prey/GameDll/ark/player/ArkPlayer.h"
#include "Prey/GameDll/ark/player/ArkPlayerComponent.h"
#include "Prey/GameDll/ark/player/ability/ArkAbilityComponent.h"
#include "Prey/CryGame/IGameFramework.h"
#include <Prey/CryGame/Game.h>
#include <Prey/GameDll/ark/ArkListenerManager.h>
#include <Prey/CryGame/IGameTokens.h>


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

    DrawMenuBar();
    DrawGameTokenViewWindow(&m_bShowGameTokenView);
    // Modders, please move to a method, it's just an example

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
    IGameTokenSystem* gameTokenSystem = gCL->cl->GetFramework()->GetIGameTokenSystem();
    if (gameTokenSystem == nullptr) {
        return;
    }

    auto player = ArkPlayer::GetInstancePtr();
    auto numberOfNeuromods = (float)player->m_playerComponent.GetAbilityComponent().GetNumNeuromodsUsed();

    IGameToken* mentalLoadToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadValue");
    // format the float to 6 decimal places
    mentalLoadToken->SetValueAsString(std::to_string(numberOfNeuromods).c_str());
}

void ModMain::OnBecomeAlien() {
    CryLog("Became alien");
}

void ModMain::DrawGameTokenViewWindow(bool *pbIsOpen) {
    if(!*pbIsOpen) return;

    IGameTokenSystem* gameTokenSystem = gCL->cl->GetFramework()->GetIGameTokenSystem();
    if (gameTokenSystem == nullptr) {
        return;
    }

    IGameToken* mentalLoadToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadValue");
    IGameToken* mentalLoadStageToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadStage");
    IGameToken* needValueToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.NeedValue");
    IGameToken* needStageToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.NeedStage");
    IGameToken* addictionTickRateToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.AddictionTickRate");


    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("Game Token View", pbIsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    if(mentalLoadToken) {
        auto value = mentalLoadToken->GetValueAsString();
        // parse the string to a float
        float numberOfNeuromods = std::stof(value.c_str());
        ImGui::Text("Mental Load Value: %s", value.c_str());
        ImGui::ProgressBar(numberOfNeuromods / 100.0f);
    } else {
        ImGui::Text("Mental Load Value: Not Found");
    }

    if(mentalLoadStageToken) {
        auto value = mentalLoadStageToken->GetValueAsString();
        ImGui::Text("Mental Load Stage: %s", value.c_str());
    } else {
        ImGui::Text("Mental Load Stage: Not Found");
    }

    if(needValueToken) {
        auto value = needValueToken->GetValueAsString();
        ImGui::Text("Need Value: %s", value.c_str());
    } else {
        ImGui::Text("Need Value: Not Found");
    }

    if(needStageToken) {
        auto value = needStageToken->GetValueAsString();
        ImGui::Text("Need Stage: %s", value.c_str());
    } else {
        ImGui::Text("Need Stage: Not Found");
    }

    if(addictionTickRateToken) {
        auto value = addictionTickRateToken->GetValueAsString();
        ImGui::Text("Addiction Tick Rate: %s", value.c_str());
    } else {
        ImGui::Text("Addiction Tick Rate: Not Found");
    }

    ImGui::Separator();


    ImGui::End();
}

void ModMain::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Synapse Junkie")) {
            ImGui::MenuItem("Show Game Token View", nullptr, &m_bShowGameTokenView);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
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
