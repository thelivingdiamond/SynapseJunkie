#pragma once
#include <Chairloader/ModSDK/ChairloaderModBase.h>
#include <Prey/GameDll/ark/iface/IArkAbilityListener.h>

class ModMain final : public ChairloaderModBase, IArkAbilityListener
{
public:
    using BaseClass = ChairloaderModBase;

    //---------------------------------------------------------------------------------
    // Mod Methods
    //---------------------------------------------------------------------------------
    
    // Put your methods here

    //---------------------------------------------------------------------------------
    // Mod Initialization
    //---------------------------------------------------------------------------------
    //! Fills in the DLL info during initialization.
    virtual void FillModInfo(ModDllInfoEx& info) override;

    //! Initializes function hooks before they are installed.
    virtual void InitHooks() override;

    //! Called during CSystem::Init, before any engine modules.
    virtual void InitSystem(const ModInitInfo& initInfo, ModDllInfo& dllInfo) override;

    //! Called after CGame::Init
    virtual void InitGame(bool isHotReloading) override;

    //---------------------------------------------------------------------------------
    // Mod Shutdown
    //---------------------------------------------------------------------------------
    //! Called before CGame::Shutdown.
    virtual void ShutdownGame(bool isHotUnloading) override;

    //! Called before CSystem::Shutdown.
    virtual void ShutdownSystem(bool isHotUnloading) override;

    //---------------------------------------------------------------------------------
    // GUI
    //---------------------------------------------------------------------------------
    //! Called just before MainUpdate to draw GUI. Only called when GUI is visible.
    virtual void Draw() override;

    //---------------------------------------------------------------------------------
    // Main Update Loop
    //---------------------------------------------------------------------------------
    //! Earliest point of update in a frame, before CGame::Update. The timer still tracks time for the previous frame.
    virtual void UpdateBeforeSystem(unsigned updateFlags) override {}

    //! Called before physics is updated for the new frame, best point for queuing physics jobs.
    //! This is like FixedUpdate() in Unity (but not FPS-independent). Use gEnv->pTimer->GetFrameTime() for time delta.
    virtual void UpdateBeforePhysics(unsigned updateFlags) override {}

    //! Called after entities have been updated but before FlowSystem and FlashUI.
    //! This is the main update where most game logic is expected to occur.
    //! Should be preferred if you don't need any special behavior.
    virtual void MainUpdate(unsigned updateFlags) override;

    //! Called after most of game logic has been updated, before CCryAction::PostUpdate.
    virtual void LateUpdate(unsigned updateFlags) override {}

    //---------------------------------------------------------------------------------
    // Mod Interfacing
    //---------------------------------------------------------------------------------
    //! Retrieves an interface for the mod.
    // virtual void* QueryInterface(const char *ifaceName) override;

    //! Called after CSystem::Init, after all engine modules and mods have been initialized. Allows your mod to get interfaces from other mods.
    // virtual void Connect(const std::vector<IChairloaderMod*>& mods) override;

private:
    void OnAbilityAdded(uint64_t _abilityID) override;

    void OnBecomeAlien() override;

    void FindStatusEnumValues();

    void OnTimerTick();

    void loadConfig();

    void UpdateMentalLoadStage();

    int GetMentalLoadStage(int numberOfNeuromods) const;

    static void AccumulateWithdrawal(float amount);

    static void AccumulateAddiction(float amount);

    EArkPlayerStatus GetWithdrawalStatusEnumValue();
    EArkPlayerStatus GetAddictionStatusEnumValue();

    static const uint64_t s_kWithdrawalTraumaId = 12348086275151114872;
    static const uint64_t s_kAddictionTraumaId = 12348086275151114871;
    static const uint64_t s_kWithdrawalSignalPackageId = 8109010857459426815;
    static const uint64_t s_kAddictionSignalPackageId = 8109010857459426816;

private:
    bool m_bShowGameTokenView = false;

    float m_fWithdrawalTickMultiplier = 1.0f;
    float m_fTimerInterval = 10.0f;
    float m_fAccumulatedTime = 0.0f;
    float m_fLastUpdateTime = 0.0f;

    int m_withdrawalStatusEnumValue = -1;
    int m_addictionStatusEnumValue = -1;

    int m_level1Threshold = 10;
    int m_level2Threshold = 20;
    int m_level3Threshold = 35;
    int m_level4Threshold = 50;
    int m_level5Threshold = 75;

    // Put private stuff here
    void DrawMenuBar();

    void DrawDebugWindow(bool* pbIsOpen);
};

extern ModMain* gMod;
