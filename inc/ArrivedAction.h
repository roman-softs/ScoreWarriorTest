#pragma once

#include "Unit.h"
#include "Types.h"
#include <functional>
#include <memory>

namespace ScoreWarrior::Test {

using ActionCallback = std::function<void(UnitID unit_id)>;

class ArrivingAction
{
public:
    ArrivingAction(const ActionCallback &cb)
        : cb_(cb)
    {

    }
    virtual ~ArrivingAction()
    {

    };

    void exec(uint64_t uuid)
    {
        (cb_)(uuid);
    }

    virtual std::optional<Coord> ranged_attack() const
    {
        return {};
    }
    virtual std::optional<uint32_t> melle_attack() const
    {
        return {};
    }
private:
    ActionCallback cb_;
};


// action of all shooting units (like Arrow of Ballista for ex)
class RangedAttack : public ArrivingAction
{
public:
    RangedAttack(Coord rad, const ActionCallback &cb)
         : ArrivingAction(cb)
         , radius_(rad)
    {

    };

    std::optional<Coord> ranged_attack() const override
    {
        return radius_;
    }

private:
    Coord radius_;
};

// action of all hand weapong units (like Warrior or Knight for ex)
class MelleAttack : public ArrivingAction {
public:
    MelleAttack(uint32_t power, const ActionCallback &cb)
        : ArrivingAction(cb)
        , power_(power)
    {

    };

    std::optional<uint32_t> melle_attack() const override
    {
        return power_;
    }

private:
    uint32_t power_;
};

} //namespace ScoreWarrior::Test
