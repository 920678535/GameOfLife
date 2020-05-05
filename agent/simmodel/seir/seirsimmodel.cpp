#include "seirsimmodel.h"
#include <random>

#include <agent/agentprovider.h>
#include <gbbinfra/descriptorregister.h>

#include <net/phoneyou/covid19/bb/Descriptors.pb.h>
#include <net/phoneyou/covid19/bb/Messages.pb.h>

SEIRSimModel::SEIRSimModel()
    : EntitySimModel("SEIR")
{
}

void SEIRSimModel::initialize()
{
    using bb::message::DimensionsChanged;
    listenerMessage<DimensionsChanged>(
        [this](const DimensionsChanged& _msg) {
            updateDimensions(_msg.cell_x(), _msg.cell_y());
        });
    using bb::message::AroundCells;
    using bb::message::AroundCellsResponse;
    registerRPCService<AroundCells, AroundCellsResponse>(
        "around_cells",
        [this](const AroundCells& _req, AroundCellsResponse& _res) {
            bb::descriptor::Position _pos;
            if (amster::StatusID::SUCCESS != readData(_req.center_id(), _pos)) {
                _res.mutable_response()->set_code(bb::ResponseCode::Failed);
                return;
            }
            auto _center = covid19::GridPt{ _pos.x(), _pos.y() };
            // 园内接正方形
            absl::ReaderMutexLock _lock(&gridMutex_);
            for (int r = 1; r <= _req.radius(); r++) {
                auto _circle = _res.mutable_circle_cells()->Add();
                _circle->set_radius(1);
                // 内切圆面积 2ΠR
                auto _ids = _circle->mutable_ids();
                _ids->Reserve(std::round(8 * std::atan(1) * r));
                const auto _inscribed = static_cast<int>(std::floor(r / std::sqrt(2)));
                for (int i = -r; i <= r; i++)
                    for (int j = -r; j <= r; j++) {
                        bool _hint{ std::abs(i) <= _inscribed
                            && std::abs(i) > r - 1
                            && std::abs(j) > r - 1
                            && std::abs(j) <= _inscribed };
                        if (!_hint) {
                            auto _d = static_cast<int>(std::round(std::hypot(i, j)));
                            _hint = r >= _d && _d > r - 1;
                        }
                        if (_hint) {
                            covid19::GridPt _pt{ _center.x + i, _center.y + j };
                            if (entityGrids_.contains(_pt.x, _pt.y))
                                _ids->Add(entityGrids_.at(_pt.x, _pt.y));
                        }
                    }
            }
            _res.mutable_response()->set_code(bb::ResponseCode::Successed);
        });
}

std::vector<EntitySimModel::DescriptorEnum> SEIRSimModel::preconditionDescriptors() const
{
    return {
        bb::DescriptorType::GridInfo,
        bb::DescriptorType::Position,
        bb::DescriptorType::Contact
    };
}

bool SEIRSimModel::beforeStep(const GBBTimeContext& _timeContext)
{
    return amster::StatusID::SUCCESS
        == readSingltonDescriptorData(settings_);
}

StepResult SEIRSimModel::step(
    const GBBTimeContext& _timeContext,
    Entity& _entity)
{
    auto _elaspedSec = _timeContext.stepSecond();
    const auto& _durations = settings_.duration();
    const auto& _rates = settings_.rate();
    auto _id = _entity.entityID();
    bb::descriptor::GridInfo _info;
    _entity.readPublishedDescriptorData(_info);
    std::random_device _rd;
    std::mt19937 _gen(_rd());
    using bb::descriptor::IncubationDuration;
    using bb::descriptor::IncubationInfectDuration;
    using bb::descriptor::InfectDuration;
    using bb::descriptor::TreatmentDuration;
    using bb::descriptor::SpontaneousActiveDuration;

    bool _sync = settings_.sync_duration_time();
    switch (_info.type()) {
    case bb::GridType::Exposed:
        if (_sync ? stepDuration<IncubationDuration>(
                        _id,
                        _elaspedSec,
                        _durations.incubation())
                  : stepDuration(
                        _id,
                        bb::DurationOptions::Incubation,
                        _elaspedSec,
                        _durations.incubation()))
            if (_rates.incubation_infected()
                > std::generate_canonical<double, 10>(_gen))
                confirmDiagnosis(_entity);

        if (_sync ? stepDuration<IncubationInfectDuration>(
                        _id,
                        _elaspedSec,
                        _durations.incubation_infect())
                  : stepDuration(
                        _id,
                        bb::DurationOptions::IncubationInfect,
                        _elaspedSec,
                        _durations.incubation_infect()))
            spread(_entity, _rates.incubation_transmission());

        break;
    case bb::GridType::Infectious:
        if (_sync ? stepDuration<InfectDuration>(
                        _id,
                        _elaspedSec,
                        _durations.infect())
                  : stepDuration(
                        _id,
                        bb::DurationOptions::Infect,
                        _elaspedSec,
                        _durations.infect()))
            spread(_entity, _rates.transmission());

        if (_sync ? stepDuration<TreatmentDuration>(
                        _id,
                        _elaspedSec,
                        _durations.treatment())
                  : stepDuration(
                        _id,
                        bb::DurationOptions::Treatment,
                        _elaspedSec,
                        _durations.treatment()))
            recover(_entity);
        break;
    case bb::GridType::Susceptible:
        if (_sync ? stepDuration<SpontaneousActiveDuration>(
                        _id,
                        _elaspedSec,
                        _durations.spontaneous_activation())
                  : stepDuration(
                        _id,
                        bb::DurationOptions::SpontaneousActivation,
                        _elaspedSec,
                        _durations.spontaneous_activation()))
            if (_rates.spontaneous_activation()
                > std::generate_canonical<double, 10>(_gen))
                confirmDiagnosis(_entity);
        break;
    default:
        break;
    }
}

void SEIRSimModel::loopDestroyed()
{
    {
        absl::MutexLock _lk(&gridMutex_);
        entityGrids_ = {};
    }
    {
        absl::MutexLock _lk(&durationMutex_);
        entityDurations_ = {};
    }
    inited_ = false;
}

void SEIRSimModel::entityExisted(EntitySimModel::EntityCreationTuple _entityTuple)
{
    entityAdded(_entityTuple);
}

void SEIRSimModel::entityAdded(EntitySimModel::EntityCreationTuple _entityTuple)
{
    initFromBB();
    bb::descriptor::Position _pos;
    auto _id = std::get<0>(_entityTuple);
    readData(_id, _pos);
    auto _success = setGridEntity({ _pos.x(), _pos.y() }, _id);
    LOGF_IF(WARNING,
        !_success,
        "cell {}'{{{}}} is found in grids.",
        _id,
        _pos.DebugString());
}

void SEIRSimModel::initFromBB()
{
    if (!descriptorInitialized(bb::DescriptorType::Dimensions, SINGLETON_DESCPRITOR_INDEX))
        return;
    bool _inited{ false };
    if (inited_.compare_exchange_strong(_inited, true)) {
        bb::descriptor::Dimensions _dim;
        readSingltonDescriptorData(_dim);
        LOGF_IF(WARNING,
            0 == _dim.cell_x() || 0 == _dim.cell_y(),
            "read Dimensions from bb is {{{}}}",
            _dim.DebugString());
        updateDimensions(_dim.cell_x(), _dim.cell_y());
    }
}

void SEIRSimModel::updateDimensions(uint32_t _cellx, uint32_t _celly)
{
    absl::MutexLock _lock(&gridMutex_);
    entityGrids_.resize(_cellx, _celly);
    LOGF(INFO, "update grid dimensions to x:{} y:{}", _cellx, _celly);
}

std::tuple<EntityId, bool> SEIRSimModel::entityAtPt(covid19::GridPt _pt) const
{
    EntityId _id{ INVALID_ENTITY_ID };
    bool _existed{ false };
    absl::ReaderMutexLock _lock(&gridMutex_);
    if ((_existed = entityGrids_.contains(_pt.x, _pt.y)))
        _id = entityGrids_.at(_pt.x, _pt.y);
    return { _id, _existed };
}

bool SEIRSimModel::setGridEntity(covid19::GridPt _pt, EntityId _entity)
{
    bool _existed{ false };
    absl::ReaderMutexLock _lock(&gridMutex_);
    if ((_existed = entityGrids_.contains(_pt.x, _pt.y)))
        entityGrids_.at(_pt.x, _pt.y) = _entity;
    return _existed;
}

void SEIRSimModel::spread(Entity& _entity, float _transmissionRate)
{
    covid19::GridPt _cellPos{};
    uint32_t _degree{ 0 };
    // get position
    {
        bb::descriptor::Position _pos;
        _entity.readPublishedDescriptorData(_pos);
        _cellPos = { _pos.x(), _pos.y() };
    }
    // get degree
    {
        bb::descriptor::Contact _contact;
        _entity.readPublishedDescriptorData(_contact);
        _degree = _contact.degree();
    }
    // infect around grid
    auto _contactPos = _cellPos.around8();
    // random the contated targets.
    int _contacted{ 0 };
    bb::message::ChangeCellStaus _req;
    bb::descriptor::GridInfo _info;
    std::random_device _rd;
    std::mt19937 _gen(_rd());
    for (auto _near : _contactPos) {
        auto _target = entityAtPt(_near);
        auto _targetId = std::get<0>(_target);
        if (!std::get<1>(_target) || 0 == _targetId)
            continue;
        // CONTATED !!!
        if (++_contacted > _degree)
            break;
        // read status
        if (amster::StatusID::SUCCESS != readData(_targetId, _info)) {
            LOGF(WARNING, "entity has not inited GridInfo.");
            continue;
        }
        if (_info.type() == bb::GridType::Susceptible
            && _transmissionRate > std::generate_canonical<double, 10>(_gen)) {
            // INFECT THE CELL !!!
            _req.set_met_id(_targetId);

            _req.set_type(settings_.sis_model()
                    ? bb::GridType::Infectious
                    : bb::GridType::Exposed);
            doRPC("change_cell_status", _req, nullptr);
            LOGF(DBUG, "Infected: [{}] -> [{}] ", _entity.entityID(), _targetId);
        }
    }
}

void SEIRSimModel::recover(Entity& _entity)
{
    const auto& _rates = settings_.rate();
    std::random_device _rd;
    std::mt19937 _gen(_rd());
    bb::message::ChangeCellStaus _req;
    _req.set_met_id(_entity.entityID());
    if (_rates.recover() > std::generate_canonical<double, 10>(_gen)) {
        if (settings_.sis_model()) {
            _req.set_type(bb::GridType::Susceptible);
        } else if (_rates.immunity() > std::generate_canonical<double, 10>(_gen)) {
            _req.set_type(bb::GridType::Recovered);
            _req.set_recovered_type(bb::RecoverType::Immunity);
        } else {
            _req.set_type(bb::GridType::Susceptible);
        }

    } else if (!settings_.sis_model()) {
        if (_rates.death() > std::generate_canonical<double, 10>(_gen)) {
            _req.set_type(bb::GridType::Recovered);
            _req.set_recovered_type(bb::RecoverType::Death);
        }
    }
    if (_req.type() != bb::GridType::Invalid)
        doRPC("change_cell_status", _req, nullptr);
}

void SEIRSimModel::confirmDiagnosis(Entity& _entity)
{
    bb::message::ChangeCellStaus _req;
    _req.set_met_id(_entity.entityID());
    _req.set_type(bb::GridType::Infectious);
    doRPC("change_cell_status", _req, nullptr);
}

template <typename DurationT>
bool SEIRSimModel::stepDuration(EntityId _id, double _elaspedSec, double _timeout)
{
    if (_timeout < 0.0001)
        return false;
    DurationT _duration;
    auto _status = readData(_id, _duration);
    if (_status != amster::StatusID::SUCCESS && _status != amster::StatusID::NOT_INITIALIZED) {
        LOGF_IF(WARNING, _status != amster::StatusID::SUCCESS && _status != amster::StatusID::NOT_INITIALIZED,
            "read descriptor [{}] from entity type [{}] error.",
            DESCRIPTOR_TYPE(DurationT), getEntityType(_id));
        return false;
    }

    bool _res = { false };
    _duration.set_elapsed_sec(_duration.elapsed_sec() + _elaspedSec);
    _duration.set_progress(_duration.elapsed_sec() / _timeout);
    if (_duration.elapsed_sec() > _timeout) {
        // reset duration
        _duration.set_elapsed_sec(0);
        _duration.set_progress(0);
        _duration.set_trigger_cnt(_duration.trigger_cnt() + 1);
        _res = true;
    }
    writeData(_id, _duration);
    return _res;
}
bool SEIRSimModel::stepDuration(
    EntityId _id,
    bb::DurationOptions::Enum _type,
    double _elaspedSec,
    double _timeout)
{
    if (_timeout < 0.0001)
        return false;
    absl::MutexLock _lk(&durationMutex_);
    auto& _durations = entityDurations_[_id];
    bool _res = { false };
    switch (_type) {
    case bb::DurationOptions::Infect:
        _durations.infect += _elaspedSec;
        if (_durations.infect > _timeout) {
            _durations.infect = 0;
            _res = true;
        }
        break;
    case bb::DurationOptions::Treatment:
        _durations.treatment += _elaspedSec;
        if (_durations.treatment > _timeout) {
            _durations.treatment = 0;
            _res = true;
        }
        break;
    case bb::DurationOptions::IncubationInfect:
        _durations.incubationInfect += _elaspedSec;
        if (_durations.incubationInfect > _timeout) {
            _durations.incubationInfect = 0;
            _res = true;
        }
        break;
    case bb::DurationOptions::Incubation:
        _durations.incubation += _elaspedSec;
        if (_durations.incubation > _timeout) {
            _durations.incubation = 0;
            _res = true;
        }
        break;
    case bb::DurationOptions::SpontaneousActivation:
        _durations.spontaneousActive += _elaspedSec;
        if (_durations.spontaneousActive > _timeout) {
            _durations.spontaneousActive = 0;
            _res = true;
        }
        break;
    default:
        break;
    }
    return _res;
}

PLUMA_INHERIT_PROVIDER(SEIRSimModel, EntitySimModel)

PLUMA_CONNECTOR bool pluma_connect(pluma::Host& host)
{
    host.add(new SEIRSimModelProvider());
    return true;
}
