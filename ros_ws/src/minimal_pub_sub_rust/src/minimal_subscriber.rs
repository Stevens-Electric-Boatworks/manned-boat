use anyhow::{Error, Result};
use rclrs::*;

fn main() -> Result<(), Error> {
    let context = Context::default_from_env()?;
    let mut executor = context.create_basic_executor();

    let node = executor.create_node("minimal_subscriber")?;

    let worker = node.create_worker::<usize>(0);
    let _subscription = worker.create_subscription::<boat_data_interfaces::msg::CANMotorData, _>(
        "motors/can_motor_data",
        move |num_messages: &mut usize, msg: boat_data_interfaces::msg::CANMotorData| {
            println!("#{} | Got the RPM of the motor as: '{}'", *num_messages, msg.rpm);
        },
    )?;

    println!("Waiting for messages on motors/can_motor_data...");
    executor.spin(SpinOptions::default()).first_error()?;
    Ok(())
}
