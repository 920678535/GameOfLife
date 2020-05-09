#include "scenarioinitagent.h"
#include <random>
#include <unordered_set>

#include <gflags/gflags.h>

#include <agent/agentprovider.h>

#include <net/phoneyou/lulu/bb/Descriptors.pb.h>
#include <net/phoneyou/lulu/bb/Messages.pb.h>
#include <net/phoneyou/lulu/bb/SingletonDescriptors.pb.h>

ScenarioInitAgent::ScenarioInitAgent()
    : AgentBase("ScenarioInitAgent")
{
}

void ScenarioInitAgent::initialize()
{
    using bb::message::ScenarioInit;
    registerRPCService<ScenarioInit, bb::Response>(
        "scenario_init", [this](const ScenarioInit& _req, bb::Response& _res) {
            // write to bb
            {
                bb::descriptor::Dimensions _dim;
                _dim.set_cell_x(_req.cell_x());
                _dim.set_cell_y(_req.cell_y());
                writeSingltonDescriptorData(_dim);
            }
            // notify all other online agents for init grid map
            {
                bb::message::DimensionsChanged _changed;
                _changed.set_cell_x(_req.cell_x());
                _changed.set_cell_y(_req.cell_y());
                publishMessage(_changed);
            }
            // TODO init cells
            {

            }
            _res.set_code(bb::ResponseCode::Successed);
        });

    using bb::message::ChangeCellStaus;
    registerRPCService<ChangeCellStaus, bb::Response>(
        "change_cell_status", [this](const ChangeCellStaus& _req, bb::Response& _res) {
            auto _id = _req.met_id();
            if (!entityExists(_id)) {
                _res.set_code(bb::ResponseCode::Failed);
                return;
            }
            // update state
            bb::descriptor::CellInfo _info;
            readData(_id, _info);
            _info.set_staus(_req.staus());
            writeData(_id, _info);
            _res.set_code(bb::ResponseCode::Successed);
        });

    using bb::message::GenerateARoom;
    registerRPCService<GenerateARoom, bb::Response>(
        "generate_a_room", [this](const GenerateARoom& _req, bb::Response& _res) {
            bb::descriptor::CellInfo _info;
            bb::descriptor::Position _pos;
            for (int i = 0; i < _req.min_degree(); ++i) {
                for (int j = 0; j < _req.max_degree(); ++j) {
                    EntityId _id;
                    // set status
                    createAgentEntity(bb::EntityType::Cell, _id);
                    _info.set_staus(bb::CellStaus::Death);
                    writeData(_id, _info);
                    // set pos
                    _pos.Clear();
                    _pos.set_x(j);
                    _pos.set_y(i);
                    writeData(_id, _pos);
                    // Ready
                    setEntityReady(_id,true);
                }
            }
            _res.set_code(bb::ResponseCode::Successed);
        });

    using bb::message::ResetAllCells;
    registerRPCService<ResetAllCells, bb::Response>(
        "reset_all_cells", [this](const ResetAllCells& _req, bb::Response& _res) {
            std::set<EntityId> _entities;
            entities(bb::EntityType::Cell, &_entities);
            bb::descriptor::CellInfo _info;
            auto _staus = _req.staus();
            for (auto _metId : _entities) {
                readData(_metId, _info);
                _info.set_staus(_staus);
                writeData(_metId, _info);
            }
            _res.set_code(bb::ResponseCode::Successed);
        });

    using bb::message::DimensionsChanged;
    listenerMessage<DimensionsChanged>(
        [this](const DimensionsChanged& _msg) {
            if (!descriptorInitialized(bb::DescriptorType::Dimensions, SINGLETON_DESCPRITOR_INDEX)) {
                return;
            }
            bb::descriptor::Dimensions _d;
            _d.set_cell_x(_msg.cell_x());
            _d.set_cell_x(_msg.cell_y());
            writeSingltonDescriptorData(_d);
        });
}

PLUMA_INHERIT_PROVIDER(ScenarioInitAgent, AgentBase)

PLUMA_CONNECTOR bool pluma_connect(pluma::Host& host)
{
    host.add(new ScenarioInitAgentProvider());
    return true;
}
