#include "SequenceApp.hh"

#include "game/CharacterSelect3D.hh"
#include "game/MenuBackground.hh"
#include "game/MenuTitleLine.hh"
#include "game/SceneFactory.hh"
#include "game/System.hh"
#include "game/SystemRecord.hh"

void SequenceApp::setNextScene(s32 sceneType) {
    s_nextScene = sceneType;
    m_state = 0;
}

void SequenceApp::Call(s32 sceneType) {
    if (sceneType == SceneType::Title) {
        // Alternate title screen
        SystemRecord::Instance().setGameFlag(SystemRecord::GameFlag::All);
    }

    REPLACED(Call)(sceneType);
}

SequenceApp *SequenceApp::Create() {
    if (!s_instance) {
        s_instance = new (System::GetAppHeap(), 0x4) SequenceApp;
    }
    return s_instance;
}

SequenceApp *SequenceApp::Instance() {
    return s_instance;
}

SequenceApp::SequenceApp()
    : GameApp(0x65f400, "Sequence", nullptr), m_scene(nullptr), m_loadingFlag(0), m_loadFlag(0),
      m_state(0), _48(0), m_scenes(nullptr) {
    SceneFactory::Create();
    if (s_nextScene != SceneType::None) {
        ready(s_nextScene);
    }
    s_scene = SceneType::None;
}

SequenceApp::~SequenceApp() {
    for (u32 i = 0; i < m_scenes.count(); i++) {
        delete m_scenes[i];
        m_scenes[i] = nullptr;
    }
    SceneFactory::Destroy();
    MenuBackground::Destroy();
    MenuTitleLine::Destroy();
    CharacterSelect3D::Destroy();
    s_nextScene = SceneType::None;
    s_scene = SceneType::None;
    s_instance = nullptr;
}

void SequenceApp::freeForMovieApp() {
    for (u32 i = 0; i < m_scenes.count(); i++) {
        delete m_scenes[i];
        m_scenes[i] = nullptr;
    }
    m_loadingFlag = 0;
    m_loadFlag = 0;
    m_scene = nullptr;
    SceneFactory::Destroy();
    MenuBackground::Destroy();
    MenuTitleLine::Destroy();
    CharacterSelect3D::Destroy();
    m_heap->freeAll();
    SceneFactory::Create();
}

Scene *SequenceApp::createScene(s32 sceneType) {
    if (!m_scenes[sceneType]) {
        m_scenes[sceneType] = SceneFactory::Instance()->createScene(sceneType, m_heap);
    }
    return m_scenes[sceneType];
}
