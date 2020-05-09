#ifndef SCENARIOINITAGENT_H
#define SCENARIOINITAGENT_H

#pragma once

#include <agent/agentbase.h>
using namespace amster::agent;
using namespace amster::gbb;

#include <net/phoneyou/lulu/bb/Common.pb.h>
namespace bb = net::phoneyou::lulu::bb;

class ScenarioInitAgent : public AgentBase {
public:
    ScenarioInitAgent();

    // AgentBase interface
public:
    void initialize() override;

private:

};

#endif // SCENARIOINITAGENT_H
