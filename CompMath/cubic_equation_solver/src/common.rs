use core::fmt;
use std::io::{self, Write};

#[derive(Debug)]
pub struct EquationSolutions {
    pub solutions: Vec<f64>,
    pub multiplicities: Vec<i32>
}

impl EquationSolutions {
    pub fn new() -> EquationSolutions {
        EquationSolutions {
            solutions: Vec::new(),
            multiplicities: Vec::new()
        }
    }

    pub fn add(&mut self, sol: f64, multiplicity: i32) {
        self.solutions.push(sol);
        self.multiplicities.push(multiplicity);
    }

    pub fn roots_count(&self) -> usize {
        self.solutions.len()
    }
}

impl fmt::Display for EquationSolutions {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        if self.solutions.len() == 0 {
           write!(f, "0 solutions found.\n")?;
           return Ok(());
        }

        write!(f, "{} solutions found:\n", self.solutions.len())?;
        let zipped = self.solutions.iter().zip(self.multiplicities.iter());
        for (i, (sol, mult)) in zipped.enumerate() {
             write!(f, "\t{}) {}, multiplicity - {}\n", i, sol, mult)?;
        }

       Ok(())
    }
}

pub fn is_zero(val: f64, eps: f64) -> bool {
    val.abs() < eps
}

pub fn get_float_input(prompt: &str) -> Result<f64, io::Error> {
    print!("{}", prompt);
    io::stdout().flush()?;
    
    let mut input = String::new();
    io::stdin().read_line(&mut input)?;
    let value: f64 = input.trim().parse()
        .map_err(|_| io::Error::new(
                io::ErrorKind::InvalidInput,
                "Please type a number!"))?;

    Ok(value)
}
