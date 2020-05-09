#include "seirsimmodel.h"
#include <random>

#include <agent/agentprovider.h>
#include <gbbinfra/descriptorregister.h>

#include <net/phoneyou/lulu/bb/Descriptors.pb.h>
#include <net/phoneyou/lulu/bb/Messages.pb.h>

SEIRSimModel::SEIRSimModel()
    : EntitySimModel("SEIR")
{
}

void SEIRSimModel::initialize()
{
}

std::vector<EntitySimModel::DescriptorEnum> SEIRSimModel::preconditionDescriptors() const
{
    return {
        bb::DescriptorType::CellInfo,
        bb::DescriptorType::Position,
        bb::DescriptorType::Statistics,
        bb::DescriptorType::Dimensions
    };
}

bool SEIRSimModel::beforeStep(const GBBTimeContext& _timeContext)
{
    return true;
}

StepResult SEIRSimModel::step(
    const GBBTimeContext& _timeContext,
    Entity& _entity)
{
    return StepResult::Done;
}

void SEIRSimModel::loopDestroyed()
{
}

void SEIRSimModel::entityExisted(EntitySimModel::EntityCreationTuple _entityTuple)
{
    entityAdded(_entityTuple);
}

void SEIRSimModel::entityAdded(EntitySimModel::EntityCreationTuple _entityTuple)
{
    bb::descriptor::Position _pos;
    auto _id = std::get<0>(_entityTuple);
    readData(_id, _pos);
    LOGF_IF(WARNING,
        true,
        "cell {}'{{{}}} is found in grids.",
        _id,
        _pos.DebugString());
}

PLUMA_INHERIT_PROVIDER(SEIRSimModel, EntitySimModel)

PLUMA_CONNECTOR bool pluma_connect(pluma::Host& host)
{
    host.add(new SEIRSimModelProvider());
    return true;
}
