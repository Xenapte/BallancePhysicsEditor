#pragma once

#include <BML/BMLAll.h>
#include <memory>

extern "C" {
  __declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class PhysicsEditor : public IMod {
public:
  PhysicsEditor(IBML* bml) : IMod(bml) {}

  virtual CKSTRING GetID() override { return "PhysicsEditor"; }
  virtual CKSTRING GetVersion() override { return "0.0.2"; }
  virtual CKSTRING GetName() override { return "Physics Editor"; }
  virtual CKSTRING GetAuthor() override { return "BallanceBug"; }
  virtual CKSTRING GetDescription() override { return "A simple mod for editing your physics settings."; }
  DECLARE_BML_VERSION;

  virtual void OnLoad() override;
  virtual void OnPostStartMenu() override;
  virtual void OnStartLevel() override;
  virtual void OnCamNavActive() override;
  virtual void OnBallNavActive() override;
  virtual void OnProcess() override;
  virtual void OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) override;

private:
  std::vector<CKBehavior*> physics_bb;
  std::unique_ptr<BGui::Label> status;
  IProperty *prop_gravity_x, *prop_gravity_y, *prop_gravity_z, *prop_time_factor, *prop_game_speed;
  const VxVector default_gravity = { 0, -20, 0 };
  VxVector gravity = default_gravity;
  static constexpr const float default_time_factor = 2;
  float time_factor = default_time_factor;
  float game_speed = 1.0f;
  CKTimeManager* time_manager;
  bool init = false;
  bool disabled = true;

  void set_physics();
  void update_config();
};
