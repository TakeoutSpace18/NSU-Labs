use crate::common::is_zero;
use crate::common::get_float_input;
use std::io::{self, Read, Write};

#[derive(Debug)]
pub struct CubicEquation {
    a: f64,
    b: f64,
    c: f64,
    d: f64,
    eps: f64,
    dt: f64
}

impl CubicEquation {
    pub fn new(coefs: &[f64; 4], eps: f64, dt: f64) -> CubicEquation {
        CubicEquation {
            a: coefs[0],
            b: coefs[1],
            c: coefs[2],
            d: coefs[3],
            eps,
            dt
        }
    }

    pub fn input_from_stdin() -> Result<CubicEquation, io::Error> {
        let a = get_float_input("Enter a: ")?;
        let b = get_float_input("Enter b: ")?;
        let c = get_float_input("Enter c: ")?;
        let d = get_float_input("Enter d: ")?;
        let eps = get_float_input("Enter epsilon: ")?;
        let dt = get_float_input("Enter delta: ")?;

        Ok(CubicEquation{ a, b, c, d, eps, dt })
    }

    fn normalize(&mut self) -> Result<(), &str>{
        if is_zero(self.a, self.eps) {
            return Err("coefficient a can't be zero!");
        }

        self.b /= self.a;
        self.c /= self.a;
        self.d /= self.a;
        self.a = 1.0;

        Ok(())
    }
}
