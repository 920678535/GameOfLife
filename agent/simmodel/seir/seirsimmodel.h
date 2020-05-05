#ifndef SEIRSIMMODEL_H
#define SEIRSIMMODEL_H

#pragma once

#include <absl/synchronization/mutex.h>

#include <agent/entitysimmodel.h>
using namespace amster::agent;
using namespace amster::gbb;

#include <net/phoneyou/covid19/bb/SingletonDescriptors.pb.h>
namespace bb = net::phoneyou::covid19::bb;

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
    void initFromBB();
    void updateDimensions(uint32_t _cellx, uint32_t _celly);
    std::tuple<EntityId , bool> entityAtPt(covid19::GridPt _pt)const;
    bool setGridEntity(covid19::GridPt _pt , EntityId _entity);
    template<typename DurationT >
    bool stepDuration(EntityId _id,double _elaspedSec, double _timeout);
    bool stepDuration(
            EntityId _id,
            bb::DurationOptions::Enum _type,
            double _elaspedSec,
            double _timeout);
private:
    // 确诊
    void confirmDiagnosis(Entity& _entity);
    // 传染
    void spread(Entity& _entity, float _transmissionRate);
    // 治疗
    void recover(Entity& _entity);
private:
    mutable absl::Mutex gridMutex_;
    using EntityGrid = covid19::Grid<EntityId>;
    EntityGrid entityGrids_;
    std::atomic_bool inited_;
    bb::descriptor::LemologyModel settings_;
    struct Duration{
        float incubation = {0};
        float incubationInfect = {0};
        float infect = {0};
        float treatment = {0};
        float spontaneousActive = {0};
    };
    absl::Mutex durationMutex_;
    std::unordered_map<EntityId,Duration> entityDurations_;
};

#endif // SEIRSIMMODEL_H
