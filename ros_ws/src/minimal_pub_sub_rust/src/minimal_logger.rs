use anyhow::{Error, Result};
use rclrs::*;

fn main() -> Result<(), Error> {
    let context = Context::default_from_env()?;
    let mut executor = context.create_basic_executor();
    let node = executor.create_node("log_test_node")?;
    log!(node.error(), "This is a log message!");
    executor.spin(SpinOptions::default());
    Ok(())
}
