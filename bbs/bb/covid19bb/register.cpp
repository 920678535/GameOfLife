#include <gbbinfra/entityregister.h>
#include <gbbinfra/descriptorregister.h>

#include "net/phoneyou/covid19/bb/Descriptors.pb.h"
using namespace net::phoneyou::covid19::bb::descriptor;
REGISTER_DESCRIPTORTABLE(Position)
REGISTER_DESCRIPTORTABLE(Contact)
REGISTER_DESCRIPTORTABLE(GridInfo)
REGISTER_DESCRIPTORTABLE(SpontaneousActiveDuration)
REGISTER_DESCRIPTORTABLE(IncubationDuration)
REGISTER_DESCRIPTORTABLE(InfectDuration)
REGISTER_DESCRIPTORTABLE(TreatmentDuration)
REGISTER_DESCRIPTORTABLE(IncubationInfectDuration)

#include "net/phoneyou/covid19/bb/SingletonDescriptors.pb.h"
REGISTER_SINGLETON_DESCRIPTOR(Statistics)
REGISTER_SINGLETON_DESCRIPTOR(LemologyModel)
REGISTER_SINGLETON_DESCRIPTOR(Dimensions)

#include "net/phoneyou/covid19/bb/Entities.pb.h"
using namespace net::phoneyou::covid19::bb::entity;
REGISTER_ENTITY(Grid)
