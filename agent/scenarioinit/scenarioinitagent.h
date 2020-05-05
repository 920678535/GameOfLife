#ifndef SCENARIOINITAGENT_H
#define SCENARIOINITAGENT_H

#pragma once

#include <agent/agentbase.h>
using namespace amster::agent;
using namespace amster::gbb;

#include <net/phoneyou/covid19/bb/Common.pb.h>
namespace bb = net::phoneyou::covid19::bb;

class ScenarioInitAgent : public AgentBase {
public:
    ScenarioInitAgent();

    // AgentBase interface
public:
    void initialize() override;
private:
    void updateDefaultDegree(uint32_t _newDegree);
    void updateGridType(bb::GridType::Enum _type);
    void clearDurations();
    using CircleCell = std::vector<EntityId>;
    std::vector<CircleCell> rangeCells(
            EntityId _center,
            uint32_t _radius);
};

#endif // SCENARIOINITAGENT_H
