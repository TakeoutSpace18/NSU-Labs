mod common;
mod cubic_equation;
mod quadratic_equation;

use quadratic_equation::QuadraticEquation;
use cubic_equation::CubicEquation;


fn main() -> Result<(), String> {
    let mut quadratic = QuadraticEquation::input_from_stdin()
        .map_err(|e| format!("Failed to read equation data: {:?}", e))?;

    quadratic.normalize()
        .map_err(|e| format!("Failed to normalize equation: {}", e))?;

    Ok(())
}
