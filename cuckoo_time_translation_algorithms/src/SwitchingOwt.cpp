#include <cuckoo_time_translator/SwitchingOwt.h>

#include <cuckoo_time_translator/AbstractAssert.h>

#include <ostream>

namespace cuckoo_time_translator {

SwitchingOwt::~SwitchingOwt() {
}

SwitchingOwt::SwitchingOwt(double switchingTimeSeconds, std::function<std::unique_ptr<OneWayTranslator>()> owtFactory) :
  switchingTimeSeconds_(switchingTimeSeconds)
{
  for(auto & owt : oneWayTranslators_){
    owt = owtFactory();
  }
}

const OneWayTranslator& SwitchingOwt::getCurrentOwt() const {
  return *oneWayTranslators_[0];
}
const OneWayTranslator& SwitchingOwt::getPendingOwt() const {
  return *oneWayTranslators_[1];
}
OneWayTranslator& SwitchingOwt::getCurrentOwt() {
  return *oneWayTranslators_[0];
}
OneWayTranslator& SwitchingOwt::getPendingOwt() {
  return *oneWayTranslators_[1];
}

LocalTime SwitchingOwt::translateToLocalTimestamp(RemoteTime remoteTimeTics) const {
  return getCurrentOwt().translateToLocalTimestamp(remoteTimeTics);
}

LocalTime SwitchingOwt::updateAndTranslateToLocalTimestamp(RemoteTime remoteTimeTics, LocalTime localTimeSecs) {
  AASSERT_GE(localTimeSecs, 0, "Local times must be nonnegative.");
  if(lastSwitchTime_ == -1){
    lastSwitchTime_ = localTimeSecs;
  }
  if(localTimeSecs >= lastSwitchTime_ + switchingTimeSeconds_){
    switchOwts();
    lastSwitchTime_ = localTimeSecs;
  }
  getPendingOwt().updateAndTranslateToLocalTimestamp(remoteTimeTics, localTimeSecs);
  return getCurrentOwt().updateAndTranslateToLocalTimestamp(remoteTimeTics, localTimeSecs);
}

bool SwitchingOwt::isReady() const {
  return getCurrentOwt().isReady();
}

void SwitchingOwt::printNameAndConfig(std::ostream& o) const {
  o << "SwitchingOwt(switchingTimeSecs=" << switchingTimeSeconds_ << ", currentOwt=";
  getCurrentOwt().printNameAndConfig(o);
  o << ")";
}

void SwitchingOwt::printState(std::ostream& o) const {
  o << "currentOwt:";
  getCurrentOwt().printState(o);
  o << ", pendingOwt:";
  getPendingOwt().printState(o);
}

void SwitchingOwt::reset() {
  lastSwitchTime_ = -1;
  switchCount_ = 0;
  for(auto & owt : oneWayTranslators_){
    owt->reset();
  }
}

void SwitchingOwt::switchOwts() {
  switchCount_ ++;
  getCurrentOwt().reset();
  std::swap(oneWayTranslators_[0], oneWayTranslators_[1]);
}

} /* namespace cuckoo_time_translator */
