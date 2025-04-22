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
    gCL->cl->GetFramework()->RegisterListener(this, "SynapseJunkie",
                                              EFRAMEWORKLISTENERPRIORITY::FRAMEWORKLISTENERPRIORITY_DEFAULT);

}

//---------------------------------------------------------------------------------
// Mod Shutdown
//---------------------------------------------------------------------------------
void ModMain::ShutdownGame(bool isHotUnloading) {
    // Unregister the listener
    g_pGame->m_pArkListenerManager->UnregisterAbilityListener(this);
    gCL->cl->GetFramework()->UnregisterListener(this);
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
    DrawGameTokenViewWindow(&m_bShowGameTokenView);
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

void ModMain::OnPostUpdate(float) {

}

// TODO: WHY DOES THIS WORK?????
static char a = 'a';

void ModMain::OnSaveGame(ISaveGame *saveGame) {

    // TODO: WHY MAKE IT FROM a?
    TSerialize *ser = saveGame->AddSection((TSerialize *) &a, "SynapseJunkie");

    int randNum = rand();
    ser->Value("randomNumber", randNum);
    CryLog("Saved random number: {}", randNum);
}

void ModMain::OnLoadGame(ILoadGame *loadGame) {
    if (loadGame != nullptr) {
        if (!loadGame->HaveSection("SynapseJunkie")) {
            return;
        }

        // TODO: WHY MAKE IT FROM a? WHY DOES THIS WORK????
        std::unique_ptr<TSerialize> serPtr = std::make_unique<TSerialize>((ISerialize *) &a);
        auto b = loadGame->GetSection(&serPtr, "SynapseJunkie");
        if (b == nullptr) {
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
    IGameTokenSystem *gameTokenSystem = gCL->cl->GetFramework()->GetIGameTokenSystem();
    if (gameTokenSystem == nullptr) {
        return;
    }

    auto player = ArkPlayer::GetInstancePtr();
    auto numberOfNeuromods = (float) player->m_playerComponent.GetAbilityComponent().GetNumNeuromodsUsed();

    IGameToken *mentalLoadToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadValue");
    IGameToken *mentalLoadStageToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadStage");

    mentalLoadToken->SetValue(TFlowInputData(numberOfNeuromods));
    int mentalLoadStage = GetMentalLoadStage(numberOfNeuromods);
    mentalLoadStageToken->SetValue(TFlowInputData(mentalLoadStage));

}

void ModMain::OnBecomeAlien() {
    CryLog("Became alien");
}

void ModMain::DrawGameTokenViewWindow(bool *pbIsOpen) {
    if (!*pbIsOpen) return;

    IGameTokenSystem *gameTokenSystem = gCL->cl->GetFramework()->GetIGameTokenSystem();
    if (gameTokenSystem == nullptr) {
        return;
    }


    IGameToken *mentalLoadToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadValue");
    IGameToken *mentalLoadStageToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadStage");
    IGameToken *needValueToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.NeedValue");
    IGameToken *needStageToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.NeedStage");
    IGameToken *addictionTickRateToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.AddictionTickRate");


    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("Game Token View", pbIsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    // indicators for is game started/paused
//    gCL->cl->GetFramework()->IsGameStarted();
//    gCL->cl->GetFramework()->IsGamePaused();
    ImGui::Text("Game Started: %s", gCL->cl->GetFramework()->IsGameStarted() ? "true" : "false");
    ImGui::Text("Game Paused: %s", gCL->cl->GetFramework()->IsGamePaused() ? "true" : "false");


    ImGui::Separator();

    // now we wanna display the amount of accumulated need/debuff
    auto pPlayer = ArkPlayer::GetInstancePtr();
    if (pPlayer != nullptr) {
//        auto statusComponent = pPlayer->m_playerComponent.GetStatusComponent();
        float amount = 0.0f;
        for (const auto &item: ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->m_statuses) {
            if (item.get()->m_id == 12348086275151114872) {
                amount = item.get()->m_currentAmount;
            }
        }
        ImGui::Text("Accumulated Need: %f", amount);
        ImGui::ProgressBar(amount / 10000.0f);
    }


    if (mentalLoadToken) {
        auto value = mentalLoadToken->GetValueAsString();
        // parse the string to a float
        float numberOfNeuromods = std::stof(value.c_str());
        ImGui::Text("Mental Load Value: %s", value.c_str());
        ImGui::ProgressBar(numberOfNeuromods / 75.0f);
    } else {
        ImGui::Text("Mental Load Value: Not Found");
    }

    if (mentalLoadStageToken) {
        auto value = mentalLoadStageToken->GetValueAsString();
        ImGui::Text("Mental Load Stage: %s", value.c_str());
    } else {
        ImGui::Text("Mental Load Stage: Not Found");
    }

    if (needValueToken) {
        auto value = needValueToken->GetValueAsString();
        ImGui::Text("Need Value: %s", value.c_str());
    } else {
        ImGui::Text("Need Value: Not Found");
    }

    if (needStageToken) {
        auto value = needStageToken->GetValueAsString();
        ImGui::Text("Need Stage: %s", value.c_str());
    } else {
        ImGui::Text("Need Stage: Not Found");
    }

    if (addictionTickRateToken) {
        auto value = addictionTickRateToken->GetValueAsString();
        ImGui::Text("Addiction Tick Rate: %s", value.c_str());
    } else {
        ImGui::Text("Addiction Tick Rate: Not Found");
    }

    ImGui::Separator();

    // add a button to send a signal to the player
    if (ImGui::Button("Test Signal")) {
        auto pArkGame = ArkGame::GetArkGame();
        if (pArkGame != nullptr) {
            auto pArkSignalManager = pArkGame->GetArkSignalManager();
            int playerId = ArkPlayer::GetInstance().GetEntityId();
            auto package = pArkSignalManager.GetPackageData(8109010857459426815);
//            ArkSignalSystem::Package aPackage
            pArkSignalManager.SendPackage(playerId, 0, 0, 8109010857459426815, ArkSignalSystem::CArkSignalContext(),
                                          1.0, 0, 0, false);
        }
    }

    if (ImGui::Button("Remove Our Shit")) {
        int statusValue = 0;
        for (const auto &item: ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->m_statuses) {
            if (item.get()->m_id == 12348086275151114872) {
                statusValue = (int) item.get()->m_status;
            }
        }
        if (statusValue != 0) {
            // enum futzing because it's custom
            ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->RemoveStatus((EArkPlayerStatus) statusValue);
        }
    }

    if (ImGui::Button("Print out our status number")) {
        for (const auto &item: ArkPlayer::GetInstance().m_playerComponent.m_pStatusComponent->m_statuses) {

            if (item.get()->m_id == 12348086275151114872) {
                CryLog("Enum Value: {}", (int) item.get()->m_status);
            }
        }
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

void ModMain::OnTimerTick() {
    ArkGame *pArkGame = ArkGame::GetArkGame();
    ArkPlayer *pPlayer = ArkPlayer::GetInstancePtr();
    IGameTokenSystem *gameTokenSystem = gCL->cl->GetFramework()->GetIGameTokenSystem();
    if (pArkGame == nullptr || pPlayer == nullptr || gameTokenSystem == nullptr) {
        return;
    }

    IGameToken *mentalLoadStageToken = gameTokenSystem->FindToken("GT_Global.SynapseJunkie.MentalLoadStage");
    auto aValue = mentalLoadStageToken->GetValue();
    int level = 0;
    aValue.GetValueWithConversion(level);
    if (level == 0) {
        return;
    }
    auto pArkSignalManager = pArkGame->GetArkSignalManager();
    auto tick_amount = cry_random(0.1f, 1.0f) * m_fTimerInterval * GetTickMultiplier(level);
    pArkSignalManager.SendPackage(pPlayer->GetEntityId(), 0, 0, 8109010857459426815,
                                  ArkSignalSystem::CArkSignalContext(), tick_amount, 0, 0, false);
}

float ModMain::GetTickMultiplier(int level) {
    //TODO: load this stuff from the config
    switch (level) {
        case 0:
            return 0.0f;
        case 1:
            return 1.0f;
        case 2:
            return 1.5f;
        case 3:
            return 2.0f;
        case 4:
            return 3.0f;
        case 5:
            return 6.0f;
        default:
            return 0.0f;
    }
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
