#include "scenarioinitagent.h"
#include <random>
#include <unordered_set>

#include <gflags/gflags.h>

#include <agent/agentprovider.h>

#include <net/phoneyou/covid19/bb/Descriptors.pb.h>
#include <net/phoneyou/covid19/bb/Messages.pb.h>
#include <net/phoneyou/covid19/bb/SingletonDescriptors.pb.h>

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
            // update model
            {
                bb::descriptor::LemologyModel _model;
                _model.mutable_rate()->CopyFrom(_req.rate());
                _model.mutable_duration()->CopyFrom(_req.duration());
                _model.set_sis_model(_req.sis_model());
                _model.set_contact_degree(_req.contact_degree());
                _model.set_inited(true);
                _model.set_sync_duration_time(_req.sync_duration_time());
                writeSingltonDescriptorData(_model);
                bb::message::LemologyModelInited _msg;
                publishMessage(_msg);
            }
            // init grid cells
            std::unordered_map<uint32_t, EntityId> _grids;
            auto posIndex = [_cellx = _req.cell_x()](uint32_t x, uint32_t y)
            {
                return y * _cellx + x;
            };
            std::unordered_set<uint32_t> _s, _i, _r;
            std::unordered_map<uint32_t, uint32_t> _degrees;
            for (const auto& _item : _req.immunity_cells())
                _i.insert(posIndex(_item.x(), _item.y()));
            for (const auto& _item : _req.immunity_cells())
                _r.insert(posIndex(_item.x(), _item.y()));
            for (const auto& _item : _req.cell_degrees())
                _degrees[posIndex(_item.pos().x(), _item.pos().y())] = _item.degree();

            bb::descriptor::GridInfo _info;
            bb::descriptor::Position _pos;
            bb::descriptor::Contact _contact;
            for (int i = 0; i < _req.cell_y(); ++i) {
                for (int j = 0; j < _req.cell_x(); ++j) {
                    EntityId _id;
                    // set status
                    createAgentEntity(bb::EntityType::Grid, _id);
                    auto _index = posIndex(j, i);
                    auto _type = _i.find(_index) != _i.end()
                        ? bb::GridType::Infectious
                        : _r.find(_index) != _r.end()
                            ? bb::GridType::Recovered
                            : bb::GridType::Susceptible;
                    _info.set_type(_type);
                    writeData(_id, _info);
                    // set pos
                    _pos.Clear();
                    _pos.set_x(j);
                    _pos.set_y(i);
                    writeData(_id, _pos);
                    auto _if = _degrees.find(_index);
                    if (_if != _degrees.end()) {
                        _contact.set_degree(_if->second);
                        _contact.set_special_degree(true);
                    } else {
                        _contact.set_degree(_req.contact_degree());
                    }
                    writeData(_id, _contact);
                    // set entity ready
                    // FIXME (new version API)
//                    setEntityReady(_id,true,true);
                    setEntityReady(_id,true,true);
                }
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
            for (const auto& _circle : rangeCells(_req.met_id(), _req.radius()))
                for (auto _id : _circle) {
                    bb::descriptor::GridInfo _info;
                    readData(_id, _info);
                    _info.set_type(_req.type());
                    _info.set_recovered_type(_req.recovered_type());
                    writeData(_id, _info);
                }
            _res.set_code(bb::ResponseCode::Successed);
        });

    using bb::message::GenerateACity;
    registerRPCService<GenerateACity, bb::Response>(
        "generate_a_city", [this](const GenerateACity& _req, bb::Response& _res) {
            auto _cells = rangeCells(_req.center_id(), _req.radius());
            float _offset = (_req.max_degree() - _req.min_degree()) * 1.0 / _cells.size();
            bb::descriptor::Contact _contact;
            float _degree = _req.max_degree();
            for (auto _circle : _cells) {
                for (auto _id : _circle) {
                    readData(_id, _contact);
                    _contact.set_special_degree(true);
                    _contact.set_degree(std::round(_degree));
                    writeData(_id, _contact);
                }
                _degree -= _offset;
            }
            // _cells contains radius == 0
            _res.set_code(_cells.size() > _req.radius()
                    ? bb::ResponseCode::Successed
                    : bb::ResponseCode::Failed);
        });

    using bb::message::UpdateLemologyModel;
    registerRPCService<UpdateLemologyModel, bb::Response>(
        "update_lemology_model", [this](const UpdateLemologyModel& _req, bb::Response& _res) {
            bb::descriptor::LemologyModel _model;
            readSingltonDescriptorData(_model);
            for (auto _option : _req.options()) {
                switch (_option) {
                case UpdateLemologyModel::ChangeSISModel:
                    _model.set_sis_model(_req.sis_model());
                    break;
                case UpdateLemologyModel::ChangeSyncDurationTime: {
                    _model.set_sync_duration_time(_req.sync_duration_time());
                    if (!_req.sync_duration_time())
                        clearDurations();
                } break;
                case UpdateLemologyModel::ChangeDeathRate:
                    _model.mutable_rate()->set_death(_req.rate().death());
                    break;
                case UpdateLemologyModel::ChangeRecoverRate:
                    _model.mutable_rate()->set_recover(_req.rate().recover());
                    break;
                case UpdateLemologyModel::ChangeImmunityRate:
                    _model.mutable_rate()->set_immunity(_req.rate().immunity());
                    break;
                case UpdateLemologyModel::ChangeContactDegree: {
                    updateDefaultDegree(_req.contact_degree());
                    _model.set_contact_degree(_req.contact_degree());
                } break;
                case UpdateLemologyModel::ChangeInfectDuration:
                    _model.mutable_duration()->set_infect(_req.duration().infect());
                    break;
                case UpdateLemologyModel::ChangeTransmissionRate:
                    _model.mutable_rate()->set_transmission(_req.rate().transmission());
                    break;
                case UpdateLemologyModel::ChangeTreatmentDuration:
                    _model.mutable_duration()->set_treatment(_req.duration().treatment());
                    break;
                case UpdateLemologyModel::ChangeIncubationDuration:
                    _model.mutable_duration()->set_incubation(_req.duration().incubation());
                    break;
                case UpdateLemologyModel::ChangeIncubationInfectedRate:
                    _model.mutable_rate()->set_incubation_infected(_req.rate().incubation_infected());
                    break;
                case UpdateLemologyModel::ChangeIncubationInfectDuration:
                    _model.mutable_duration()->set_incubation_infect(_req.duration().incubation_infect());
                    break;
                case UpdateLemologyModel::ChangeSpontaneousActivationRate:
                    _model.mutable_rate()->set_spontaneous_activation(_req.rate().spontaneous_activation());
                    break;
                case UpdateLemologyModel::ChangeIncubationTransmissionRate:
                    _model.mutable_rate()->set_incubation_transmission(_req.rate().incubation_transmission());
                    break;
                case UpdateLemologyModel::ChangeSpontaneousActivationDuration:
                    _model.mutable_duration()->set_spontaneous_activation(_req.duration().spontaneous_activation());
                    break;
                default:
                    break;
                }
            }
            if (_req.options_size() > 0)
                writeSingltonDescriptorData(_model);
            _res.set_code(bb::ResponseCode::Successed);
        });

    using bb::message::ResetAllCells;
    registerRPCService<ResetAllCells, bb::Response>(
        "reset_all_cells", [this](const ResetAllCells& _req, bb::Response& _res) {
            updateGridType(_req.type());
        });
}

void ScenarioInitAgent::updateDefaultDegree(uint32_t _newDegree)
{
    std::set<EntityId> _entities;
    entities(bb::EntityType::Grid, &_entities);
    bb::descriptor::Contact _contact;
    for (auto _entity : _entities) {
        readData(_entity, _contact);
        if (!_contact.special_degree()
            && _contact.degree() != _newDegree) {
            _contact.set_degree(_newDegree);
            writeData(_entity, _contact);
        }
    }
}

void ScenarioInitAgent::updateGridType(bb::GridType::Enum _type)
{
    std::set<EntityId> _entities;
    entities(bb::EntityType::Grid, &_entities);
    bb::descriptor::GridInfo _info;
    for (auto _entity : _entities) {
        readData(_entity, _info);
        _info.set_type(_type);
        writeData(_entity, _info);
    }
}

void ScenarioInitAgent::clearDurations()
{
    std::set<EntityId> _entities;
    entities(bb::EntityType::Grid, &_entities);
    for (auto _entity : _entities) {
        writeData(_entity, bb::descriptor::SpontaneousActiveDuration{});
        writeData(_entity, bb::descriptor::IncubationDuration{});
        writeData(_entity, bb::descriptor::InfectDuration{});
        writeData(_entity, bb::descriptor::TreatmentDuration{});
        writeData(_entity, bb::descriptor::IncubationInfectDuration{});
    }
}

std::vector<ScenarioInitAgent::CircleCell>
ScenarioInitAgent::rangeCells(
    EntityId _center,
    uint32_t _radius)
{
    using bb::message::AroundCells;
    using bb::message::AroundCellsResponse;
    using amster::gateway::RPCResultState;
    std::vector<CircleCell> _cells;
    _cells.push_back(CircleCell{ _center });
    AroundCells _req;
    _req.set_center_id(_center);
    _req.set_radius(_radius);
    doRPC<AroundCellsResponse>("around_cells",
        _req, [&_cells](const AroundCellsResponse _res, RPCResultState _state) {
            if (RPCResultState::Success != _state) {
                return;
            }
            _cells.resize(1 + _res.circle_cells_size());
            int i = 1;
            for (const auto& _circle : _res.circle_cells()) {
                auto& _array = _cells[i++];
                _array.reserve(_circle.ids_size());
                for (auto _id : _circle.ids())
                    _array.push_back(_id);
            }
        });
    return _cells;
}

PLUMA_INHERIT_PROVIDER(ScenarioInitAgent, AgentBase)

PLUMA_CONNECTOR bool pluma_connect(pluma::Host& host)
{
    host.add(new ScenarioInitAgentProvider());
    return true;
}
