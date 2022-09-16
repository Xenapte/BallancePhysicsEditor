#include "main.h"

IMod* BMLEntry(IBML* bml) {
  return new PhysicsEditor(bml);
}

void PhysicsEditor::set_physics() {
  for (const auto& bb: physics_bb) {
    auto* gravity_param = bb->GetInputParameter(0)->GetDirectSource();
    gravity_param->SetValue(&gravity, sizeof(gravity));
    if (bb->GetInputParameterCount() > 1) {
      auto* time_param = bb->GetInputParameter(1)->GetDirectSource();
      time_param->SetValue(&time_factor, sizeof(time_factor));
    }
    bb->ActivateInput(0);
    bb->Execute(0);
  }
}

void PhysicsEditor::update_config() {
  gravity.x = prop_gravity_x->GetFloat();
  gravity.y = prop_gravity_y->GetFloat();
  gravity.z = prop_gravity_z->GetFloat();
  time_factor = prop_time_factor->GetFloat();
  game_speed = prop_game_speed->GetFloat();

  disabled = !(gravity != default_gravity || time_factor != default_time_factor || game_speed != 1.0f);

  if (disabled) {
    status->SetVisible(false);
  }
  else {
    std::string text = "PhysicsEditor";
    if (gravity != default_gravity) {
      std::string gravity_text(128, 0);
      gravity_text.resize(snprintf(gravity_text.data(), 128, " | Gravity: %.4g, %.4g (%.4g%%), %.4g",
                                  gravity.x, gravity.y, gravity.y / default_gravity.y * 100, gravity.z));
      text += gravity_text;
    }
    if (time_factor != default_time_factor) {
      std::string time_text(64, 0);
      time_text.resize(snprintf(time_text.data(), 64, " | Time factor: %.4gx (%.4g%%)",
                               time_factor, time_factor / default_time_factor * 100));
      text += time_text;
    }
    if (game_speed != 1.0f) {
      std::string text_game_speed(64, 0);
      text_game_speed.resize(snprintf(text_game_speed.data(), 64, " | Game speed: %.4gx",
                                 game_speed));
      text += text_game_speed;
    }
    status->SetText(text.c_str());
    status->SetVisible(true);
  }
  status->Process();
}

void PhysicsEditor::OnLoad() {
  GetConfig()->SetCategoryComment("Gravity", "Gravity of the game world.");
  prop_gravity_x = GetConfig()->GetProperty("Gravity", "X");
  prop_gravity_x->SetComment("Gravity on the X-axis. Default: 0.");
  prop_gravity_x->SetDefaultFloat(0);
  prop_gravity_y = GetConfig()->GetProperty("Gravity", "Y");
  prop_gravity_y->SetComment("Gravity on the Y-axis. Default: -20.");
  prop_gravity_y->SetDefaultFloat(-20);
  prop_gravity_z = GetConfig()->GetProperty("Gravity", "Z");
  prop_gravity_z->SetComment("Gravity on the Z-axis. Default: 0.");
  prop_gravity_z->SetDefaultFloat(0);

  GetConfig()->SetCategoryComment("Time", "Time of the game and its world.");
  prop_time_factor = GetConfig()->GetProperty("Time", "PhysicsTimeFactor");
  prop_time_factor->SetComment("Physics time factor of the game. Default: 2. (not 1 !!!)");
  prop_time_factor->SetDefaultFloat(2);
  prop_game_speed = GetConfig()->GetProperty("Time", "GlobalGameSpeed");
  prop_game_speed->SetComment("Global game speed scale (will affect everything). Default: 1.");
  prop_game_speed->SetDefaultFloat(1);

  status = std::make_unique<BGui::Label>("PhysicsStatus");
  status->SetAlignment(ALIGN_BOTTOM);
  status->SetPosition({ 0.5f, 0.998f });
  status->SetText("");
  status->SetFont(ExecuteBB::GAMEFONT_03);
  status->SetZOrder(20);
  status->SetVisible(false);
  update_config();
}

void PhysicsEditor::OnPostLoadLevel() {
  if (init)
    return;

  CKBehavior* script;

  script = m_bml->GetScriptByName("Gameplay_Ingame");
  physics_bb.push_back(ScriptHelper::FindFirstBB(ScriptHelper::FindFirstBB(script,
    "Init Ingame"), "Set Physics Globals"));

  script = m_bml->GetScriptByName("Gameplay_Refresh");
  physics_bb.push_back(ScriptHelper::FindFirstBB(ScriptHelper::FindFirstBB(script,
    "Tutorial freeze/unfreeze"), "Set Physics Globals"));

  script = m_bml->GetScriptByName("Gameplay_Tutorial");
  auto* kapitel_aktion = ScriptHelper::FindFirstBB(script, "Kapitel Aktion");
  physics_bb.push_back(ScriptHelper::FindFirstBB(ScriptHelper::FindFirstBB(kapitel_aktion,
    "Tut continue/exit"), "Set Physics Globals"));
  physics_bb.push_back(ScriptHelper::FindFirstBB(ScriptHelper::FindFirstBB(kapitel_aktion,
    "MoveKeys"), "Set Physics Globals"));

  script = m_bml->GetScriptByName("Event_handler");
  physics_bb.push_back(ScriptHelper::FindFirstBB(
    ScriptHelper::FindFirstBB(script, "Unpause Level"), "Set Physics Globals"));
  physics_bb.push_back(ScriptHelper::FindFirstBB(ScriptHelper::FindFirstBB(script,
    "reset Level"), "Set Physics Globals"));

  set_physics();

  init = true;
}

void PhysicsEditor::OnPostStartMenu() {
  if (init)
    return;
  m_bml->GetTimeManager()->SetTimeScaleFactor(game_speed);
}

void PhysicsEditor::OnStartLevel() {
  if (disabled) return;
  set_physics();
}

void PhysicsEditor::OnCamNavActive() {
  if (disabled) return;
  set_physics();
  m_bml->AddTimer(1ul, [this]() { set_physics(); });
}

void PhysicsEditor::OnBallNavActive() {
  if (disabled) return;
  set_physics();
  m_bml->AddTimer(1ul, [this]() { set_physics(); });
}

void PhysicsEditor::OnProcess() {
  if (disabled) return;
  status->Process();
}

void PhysicsEditor::OnModifyConfig(C_CKSTRING category, C_CKSTRING key, IProperty* prop) {
  update_config();
  m_bml->GetTimeManager()->SetTimeScaleFactor(game_speed);
}
