mod common;
mod cubic_equation;
mod quadratic_equation;

use std::env;

use cubic_equation::CubicEquation;


fn main() -> Result<(), String> {
    let mut cubic: CubicEquation;

    match env::args().len() {
        1 => {
            cubic = CubicEquation::input_from_stdin()
                .map_err(|e| format!("Failed to read equation data: {:?}", e))?;
        }
        7 => {
            let nums_strings: Vec<String> = env::args().skip(1).collect();
            let mut nums: Vec<f64> = Vec::new();
            for s in nums_strings {
                match s.parse::<f64>() {
                    Ok(num) => nums.push(num),
                    Err(err) => panic!("Error parsing {}: {}", s, err)
                }
            }
            
            cubic = CubicEquation::new(nums[0..4].try_into().unwrap(), nums[4], nums[5]);
        }
        _ => {
            return Err(String::from("Wrong arguments count"));
        }
    }

    println!("Equation: {}", cubic);
    let roots = cubic.solve();

    println!("{}", roots);

    Ok(())
}
