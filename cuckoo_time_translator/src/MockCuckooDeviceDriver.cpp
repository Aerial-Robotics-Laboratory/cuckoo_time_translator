#include "ros/ros.h"
#include "std_msgs/String.h"

#include "MockCuckooDeviceDriver.h"

namespace cuckoo_time_translator {
constexpr uint32_t MockCuckooDeviceDriver::kWrappingNumber;
constexpr double MockCuckooDeviceDriver::kFreq;
constexpr double MockCuckooDeviceDriver::kSkew;
constexpr double MockCuckooDeviceDriver::kOffset;

MockCuckooDeviceDriver::MockCuckooDeviceDriver(ros::NodeHandle & nh) :
  cuckooClock(kWrappingNumber, kFreq, kSkew, ros::Duration(kOffset)),
  translator(TimestampUnwrapper(kWrappingNumber, kFreq), nh)
{
}


void MockCuckooDeviceDriver::step() {
  ros::Time receiveTime;
  uint32_t deviceTime;

  cuckooClock.getNewSimulatedMeasurementTimes(deviceTime, receiveTime);

  translator.update(deviceTime, receiveTime);
}
} // namespace cuckoo_time_translator

using namespace cuckoo_time_translator;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "mock_cuckoo_device_driver");

  ros::NodeHandle nh;

  ros::Rate loop_rate(1000);

  MockCuckooDeviceDriver mcdd(nh);

  while (ros::ok())
  {
    mcdd.step();
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}

