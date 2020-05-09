#include <gbbinfra/entityregister.h>
#include <gbbinfra/descriptorregister.h>

#include "net/phoneyou/lulu/bb/Descriptors.pb.h"
using namespace net::phoneyou::lulu::bb::descriptor;
REGISTER_DESCRIPTORTABLE(Position)
REGISTER_DESCRIPTORTABLE(CellInfo)

#include "net/phoneyou/lulu/bb/SingletonDescriptors.pb.h"
REGISTER_SINGLETON_DESCRIPTOR(Statistics)
REGISTER_SINGLETON_DESCRIPTOR(Dimensions)

#include "net/phoneyou/lulu/bb/Entities.pb.h"
using namespace net::phoneyou::lulu::bb::entity;
REGISTER_ENTITY(Cell)
