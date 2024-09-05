use std::io;

use crate::common::is_zero;
use crate::common::get_float_input;

#[derive(Debug)]
pub struct QuadraticEquation{
    a: f64,
    b: f64,
    c: f64,
    eps: f64,
    dt: f64
}

impl QuadraticEquation {
    pub fn new(coefs: &[f64; 3], eps: f64, dt: f64) -> QuadraticEquation {
        QuadraticEquation {
            a: coefs[0],
            b: coefs[1],
            c: coefs[2],
            eps,
            dt
        }
    }

    pub fn input_from_stdin() -> Result<QuadraticEquation, io::Error> {
        let a = get_float_input("Enter a: ")?;
        let b = get_float_input("Enter b: ")?;
        let c = get_float_input("Enter c: ")?;
        let eps = get_float_input("Enter epsilon: ")?;
        let dt = get_float_input("Enter delta: ")?;

        Ok(QuadraticEquation{ a, b, c, eps, dt })
    }

    pub fn normalize(&mut self) -> Result<(), &str>{
        if is_zero(self.a, self.eps) {
            return Err("coefficient a can't be zero!");
        }

        self.b /= self.a;
        self.c /= self.a;
        self.a = 1.0;

        Ok(())
    }
}

