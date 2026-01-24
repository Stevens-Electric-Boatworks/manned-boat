use anyhow::{Error, Result};
// use boat_data_interfaces::msg::CANMotorData;
use rclrs::*;

fn main() -> Result<(), Error> {
    let context = Context::default_from_env()?;
    let executor = context.create_basic_executor();

    let node = executor.create_node("minimal_publisher")?;
    // let publisher = node.create_publisher::<CANMotorData>("motors/can_motor_data")?;
    // 
    // let message = CANMotorData {
    //     voltage: 18,
    //     throttle_mv: 23,
    //     throttle_percentage: 43,
    //     rpm: 123,
    //     torque: 123,
    //     motor_temp: 12,
    //     current: 271,
    //     power: 12,
    // };
    // 
    // while context.ok() {
    //     publisher.publish(&message)?;
    //     std::thread::sleep(std::time::Duration::from_millis(500));
    // }
    Ok(())
}
