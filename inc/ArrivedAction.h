#pragma once

#include "Unit.h"
#include <functional>
#include <memory>

namespace ScoreWarrior::Test {

class ArrivingAction
{
public:
    virtual ~ArrivingAction(){};
    virtual void exec(uint64_t uuid) = 0;
};

using RangedAttackCallback = std::function<void(Coord rad, uint64_t unit_id)>;
// action of all shooting units (like Arrow of Ballista for ex)
class RangedAttack : public ArrivingAction
{
public:
    RangedAttack(Coord rad, const RangedAttackCallback &cb_ptr)
         : radius_(rad)
         , cb_ptr_(cb_ptr)
    {

    };

    void exec(uint64_t uuid) override
    {
        (cb_ptr_)(radius_, uuid);
    }

private:
    Coord radius_;
    RangedAttackCallback cb_ptr_;
};

using MelleAttackCallback = std::function<void(uint32_t power,  uint64_t unit_id)>;
// action of all hand weapong units (like Warrior or Knight for ex)
class MelleAttack : public ArrivingAction {
public:
    MelleAttack(uint32_t power, const MelleAttackCallback &cb)
        : power_(power)
        , cb_(cb)
    {
        //cb_ptr_ = std::make_unique<MelleAttackCallback>(cb);
    };

    void exec(uint64_t uuid) override
    {
        (cb_)(power_, uuid);
    }

private:
    uint32_t power_;
    MelleAttackCallback cb_;
    //std::unique_ptr<MelleAttackCallback> cb_ptr_;
};

} //namespace ScoreWarrior::Test
