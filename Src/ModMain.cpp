#include "ModMain.h"
#include "Prey/CryGame/IGameFramework.h"
#include <Prey/CryFlowGraph/IFlowSystem.h>
#include <Prey/CryGame/IGameTokens.h>
#include <Prey/CryGame/Game.h>
#include <Prey/GameDll/ark/ArkGame.h>
#include <Prey/GameDll/ark/player/ArkPlayer.h>
#include <Prey/GameDll/ark/ArkListenerManager.h>
#include <Prey/GameDll/ark/signalsystem/arksignalmanager.h>
#include "Prey/GameDll/ark/player/ability/ArkAbilityComponent.h"
#include <Prey/GameDll/ark/player/ArkPlayerSignalReceiver.h>
#include <Prey/GameDll/ark/player/ArkPlayerStatusComponent.h>
#include <Prey/GameDll/ark/player/ArkPlayerUIComponent.h>
#include <Prey/GameDll/ark/player/trauma/ArkTraumaBase.h>


ModMain *gMod = nullptr;

//---------------------------------------------------------------------------------
// Mod Initialization
//---------------------------------------------------------------------------------
void ModMain::FillModInfo(ModDllInfoEx &info) {
    info.modName = "thelivingdiamond.SynapseJunkie";
    info.logTag = "SynapseJunkie";
    info.supportsHotReload = true; // TODO: Add comment/wiki link
}

void ModMain::InitHooks() {

}

void ModMain::InitSystem(const ModInitInfo &initInfo, ModDllInfo &dllInfo) {
    BaseClass::InitSystem(initInfo, dllInfo);
    // Your code goes here

}

void ModMain::InitGame(bool isHotReloading) {
    BaseClass::InitGame(isHotReloading);
    // Your code goes here
    g_pGame->m_pArkListenerManager->RegisterAbilityListener(this);

}

//---------------------------------------------------------------------------------
// Mod Shutdown
//---------------------------------------------------------------------------------
void ModMain::ShutdownGame(bool isHotUnloading) {
    // Unregister the listener
    g_pGame->m_pArkListenerManager->UnregisterAbilityListener(this);
    // Your code goes here
    BaseClass::ShutdownGame(isHotUnloading);
}

void ModMain::ShutdownSystem(bool isHotUnloading) {
    // Your code goes here
    BaseClass::ShutdownSystem(isHotUnloading);
}

//---------------------------------------------------------------------------------
// GUI
//---------------------------------------------------------------------------------
void ModMain::Draw() {

    DrawMenuBar();
    DrawDebugWindow(&m_bShowGameTokenView);
    // Modders, please move to a method, it's just an example

}

//---------------------------------------------------------------------------------
// Main Update Loop
//---------------------------------------------------------------------------------

void ModMain::MainUpdate(unsigned updateFlags) {
    bool started = gCL->cl->GetFramework()->IsGameStarted();
    bool paused = gCL->cl->GetFramework()->IsGamePaused();
    // Your code goes here
    if (!paused && started) {
        // Do something
        if (gEnv->pTimer != nullptr) {
            auto delta = gEnv->pTimer->GetFrameTime(ITimer::ETIMER_GAME);
            m_fAccumulatedTime += delta;
            if (m_fAccumulatedTime - m_fLastUpdateTime > m_fTimerInterval) {
                m_fLastUpdateTime = m_fAccumulatedTime;
                OnTimerTick();
            }
        }
    }
}


void ModMain::OnAbilityAdded(uint64_t _abilityID) {
    UpdateMentalLoadStage();
}

void ModMain::OnBecomeAlien() {
    CryLog("Became alien");
}

void ModMain::DrawDebugWindow(bool *pbIsOpen) {
    if (!*pbIsOpen) return;
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("Synapse Junkie Debug Menu", pbIsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    auto pPlayer = ArkPlayer::GetInstancePtr();
    if (pPlayer != nullptr) {
        ArkTraumaBase* withdrawalTrauma = pPlayer->m_playerComponent.m_pStatusComponent->GetTraumaForStatus((EArkPlayerStatus)GetWithdrawalStatusEnumValue());
        ArkTraumaBase* addictionTrauma = pPlayer->m_playerComponent.m_pStatusComponent->GetTraumaForStatus((EArkPlayerStatus)GetAddictionStatusEnumValue());
        if(addictionTrauma != nullptr){
            ImGui::Text("Addiction Level: %d", addictionTrauma->m_currentLevel);
        }
        if(withdrawalTrauma != nullptr){
            ImGui::Text("Accumulated Withdrawal: %f", withdrawalTrauma->m_currentAmount);
            ImGui::ProgressBar(withdrawalTrauma->m_currentAmount / withdrawalTrauma->m_maxAccumulation);
        }
    }

    ImGui::Separator();

    static float addictionAmount = 0.0f;
    ImGui::InputFloat("Addiction Amount", &addictionAmount, 0.1f, 1.0f, "%.1f");
    if (ImGui::Button("Accumulate Addiction")) {
        AccumulateAddiction(addictionAmount);
    }

    static float withdrawalAmount = 0.0f;
    ImGui::InputFloat("Withdrawal Amount", &withdrawalAmount, 0.1f, 1.0f, "%.1f");
    if(ImGui::Button("Accumulate Withdrawal")) {
        AccumulateWithdrawal(withdrawalAmount);
    }

    ImGui::Separator();

    if (ImGui::Button("Remove Withdrawal Debuff")) {
        ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->RemoveStatus(
                (EArkPlayerStatus) GetWithdrawalStatusEnumValue());
    }
    if (ImGui::Button("Re-evaluate Addiction")) {
        UpdateMentalLoadStage();
    }

    ImGui::End();
}

void ModMain::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Synapse Junkie")) {
            ImGui::MenuItem("Show Debug Menu", nullptr, &m_bShowGameTokenView);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void ModMain::OnTimerTick() {
    ArkPlayer *pPlayer = ArkPlayer::GetInstancePtr();
    if (pPlayer == nullptr) {
        return;
    }

    if (!pPlayer->m_playerComponent.m_pStatusComponent->IsStatusActive(
            (EArkPlayerStatus) GetAddictionStatusEnumValue())) {
        return;
    }

    //TODO: add multiplier from config for difficulty configuration
    auto tick_amount = cry_random(0.1f, 1.0f) * 1.0 * m_fTimerInterval;
    AccumulateWithdrawal(tick_amount);
}


int ModMain::GetMentalLoadStage(int numberOfNeuromods) {
    //TODO: load this stuff from the config
    if (numberOfNeuromods < 10) {
        return 0;
    } else if (numberOfNeuromods < 20) {
        return 1;
    } else if (numberOfNeuromods < 35) {
        return 2;
    } else if (numberOfNeuromods < 50) {
        return 3;
    } else if (numberOfNeuromods < 75) {
        return 4;
    } else {
        return 5;
    }
}

void ModMain::FindStatusEnumValues() {
    if (ArkPlayer::GetInstancePtr() == nullptr) {
        return;
    }

    for (const auto &item: ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->m_statuses) {
        uint64_t id = item.get()->m_id;
        int status = (int) item.get()->m_status;
        // withdrawal
        if (id == s_kWithdrawalTraumaId) {
            m_withdrawalStatusEnumValue = status;
        }
            // addiction
        else if (id == s_kAddictionTraumaId) {
            m_addictionStatusEnumValue = status;
        }
    }
}

EArkPlayerStatus ModMain::GetAddictionStatusEnumValue() {
    if (m_addictionStatusEnumValue == -1) {
        FindStatusEnumValues();
    }

    return (EArkPlayerStatus)m_addictionStatusEnumValue;
}

EArkPlayerStatus ModMain::GetWithdrawalStatusEnumValue() {
    if (m_withdrawalStatusEnumValue == -1) {
        FindStatusEnumValues();
    }

    return (EArkPlayerStatus)m_withdrawalStatusEnumValue;
}

void ModMain::UpdateMentalLoadStage() {
    ArkPlayer* player = ArkPlayer::GetInstancePtr();
    int numberOfNeuromods = player->m_playerComponent.GetAbilityComponent().GetNumNeuromodsUsed();

    int mentalLoadStage = GetMentalLoadStage(numberOfNeuromods);
    ArkTraumaBase* trauma = ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->GetTraumaForStatus((EArkPlayerStatus) GetAddictionStatusEnumValue());

    if(trauma == nullptr) {
        return;
    }

    if (trauma->m_currentLevel != mentalLoadStage - 1) {
        if (mentalLoadStage == 0) {
            ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->RemoveStatus(
                    (EArkPlayerStatus) GetAddictionStatusEnumValue());
        } else {
            trauma->Activate(mentalLoadStage - 1);
        }
    }
}

void ModMain::AccumulateWithdrawal(float amount) {
    ArkGame *pArkGame = ArkGame::GetArkGame();
    ArkPlayer *pPlayer = ArkPlayer::GetInstancePtr();
    if (pArkGame == nullptr || pPlayer == nullptr ) {
        return;
    }
    auto pArkSignalManager = pArkGame->GetArkSignalManager();

    pArkSignalManager.SendPackage(pPlayer->GetEntityId(), 0, 0, s_kWithdrawalSignalPackageId,
                                  ArkSignalSystem::CArkSignalContext(), amount, 0, 0, false);
}

void ModMain::AccumulateAddiction(float amount) {
    ArkGame *pArkGame = ArkGame::GetArkGame();
    ArkPlayer *pPlayer = ArkPlayer::GetInstancePtr();
    if (pArkGame == nullptr || pPlayer == nullptr ) {
        return;
    }
    auto pArkSignalManager = pArkGame->GetArkSignalManager();

    pArkSignalManager.SendPackage(pPlayer->GetEntityId(), 0, 0, s_kAddictionSignalPackageId,
                                  ArkSignalSystem::CArkSignalContext(), amount, 0, 0, false);
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
extern "C" DLL_EXPORT IChairloaderMod *ClMod_Initialize() {
    CRY_ASSERT(!gMod);
    gMod = new ModMain();
    return gMod;
}

extern "C" DLL_EXPORT void ClMod_Shutdown() {
    CRY_ASSERT(gMod);
    delete gMod;
    gMod = nullptr;
}

// Validate that declarations haven't changed
static_assert(std::is_same_v<decltype(ClMod_Initialize), IChairloaderMod::ProcInitialize>);
static_assert(std::is_same_v<decltype(ClMod_Shutdown), IChairloaderMod::ProcShutdown>);
