use std::io;
use core::fmt;

use crate::common::is_zero;
use crate::common::EquationSolutions;
use crate::common::get_float_input;

#[derive(Debug)]
pub struct QuadraticEquation{
    a: f64,
    b: f64,
    c: f64,
    eps: f64,
}

impl QuadraticEquation {
    pub fn new(coefs: &[f64; 3], eps: f64) -> QuadraticEquation {
        QuadraticEquation {
            a: coefs[0],
            b: coefs[1],
            c: coefs[2],
            eps
        }
    }

    pub fn input_from_stdin() -> Result<QuadraticEquation, io::Error> {
        let a = get_float_input("Enter a: ")?;
        let b = get_float_input("Enter b: ")?;
        let c = get_float_input("Enter c: ")?;
        let eps = get_float_input("Enter epsilon: ")?;

        Ok(QuadraticEquation{ a, b, c, eps })
    }

    pub fn solve(&self) -> EquationSolutions {
        let mut roots = EquationSolutions::new();

        let discr = self.b * self.b - 4.0 * self.a * self.c;

        if is_zero(discr, self.eps) {
            roots.add(-self.b / (2.0 * self.a), 2);
            return roots;
        }

        if discr.is_sign_positive() {
            roots.add((-self.b + discr.sqrt()) / (2.0 * self.a), 1);
            roots.add((-self.b - discr.sqrt()) / (2.0 * self.a), 1);
        }

        return roots;
    }
}

impl fmt::Display for QuadraticEquation {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}x^2 + {}x + {} = 0", self.a, self.b, self.c);
        Ok(())
    }
}
