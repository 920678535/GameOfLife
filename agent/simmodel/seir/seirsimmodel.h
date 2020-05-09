#ifndef SEIRSIMMODEL_H
#define SEIRSIMMODEL_H

#pragma once

#include <absl/synchronization/mutex.h>

#include <agent/entitysimmodel.h>
using namespace amster::agent;
using namespace amster::gbb;

#include <net/phoneyou/lulu/bb/SingletonDescriptors.pb.h>
namespace bb = net::phoneyou::lulu::bb;

#include "common.h"

class SEIRSimModel : public EntitySimModel
{
public:
    SEIRSimModel();

    // EntitySimModel interface
protected:
    void initialize() override;
    std::vector<DescriptorEnum> preconditionDescriptors() const override;
    bool beforeStep(const amster::gbb::GBBTimeContext &_timeContext) override;
    StepResult step(const amster::gbb::GBBTimeContext &_timeContext, Entity &_entity) override;
    void loopDestroyed() override;
    void entityExisted(EntityCreationTuple _entityTuple) override;
    void entityAdded(EntityCreationTuple _entityTuple) override;

private:


private:
    mutable absl::Mutex gridMutex_;
    std::atomic_bool inited_;
};

#endif // SEIRSIMMODEL_H
