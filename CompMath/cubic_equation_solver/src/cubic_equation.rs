use core::fmt;
use core::panic;
use std::thread::park;
use crate::common::is_zero;
use crate::common::get_float_input;
use crate::common::EquationSolutions;
use crate::quadratic_equation::QuadraticEquation;
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

enum Direction {
    Right,
    Left
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

        if is_zero(a, eps) {
            return Err(io::Error::new(io::ErrorKind::Other,
                    "coefficient a can't be zero!"));
        }

        Ok(CubicEquation{ a, b, c, d, eps, dt })
    }

    fn normalize(&mut self) {
        self.b /= self.a;
        self.c /= self.a;
        self.d /= self.a;
        self.a = 1.0;
    }

    fn f(&self, x: f64) -> f64 {
        self.a * x.powi(3) + self.b * x.powi(2) + self.c * x + self.d
    }

    fn find_root_at_interval(&self, a: f64, b: f64) -> Option<f64> {
        let mut left = a;
        let mut right = b;

        if (self.f(left) * self.f(right)).is_sign_positive() {
            return None;
        }

        loop {
            let mid = (left + right) / 2.0; 
            if is_zero(self.f(left), self.eps) {
                return Some(left);
            }

            if is_zero(self.f(right), self.eps) {
                return Some(right);
            }

            if is_zero(self.f(mid), self.eps) {
                return Some(mid);
            }

            if (self.f(left) * self.f(mid)).is_sign_positive() {
                left = mid;
            }
            else if (self.f(mid) * self.f(right)).is_sign_positive() {
                right = mid;
            }
        }
    }

    fn find_root_at_inf_interval(&self, start: f64, dir: Direction)
        -> Option<f64> {
        let mut stop = start; 
        while (self.f(start) * self.f(stop)).is_sign_positive() {
            match dir {
                Direction::Left => stop -= self.dt,
                Direction::Right => stop += self.dt
            }
        }

        self.find_root_at_interval(
            start.min(stop), start.max(stop))
    }

    pub fn solve(&mut self) -> EquationSolutions {
        let mut roots = EquationSolutions::new();

        self.normalize();

        let derivative = QuadraticEquation::new(
            &[3.0, 2.0 * self.b, self.c], self.eps);

        let derivative = derivative.solve();

        match derivative.roots_count() {
            0 => {
                let root;
                if self.f(0.0).is_sign_positive() {
                    root = self.find_root_at_inf_interval(0.0, Direction::Left).unwrap();
                } else {
                    root = self.find_root_at_inf_interval(0.0, Direction::Right).unwrap();
                }
                roots.add(root, 1);
            }
            1 => {
                let x = derivative.solutions[0];
                if is_zero(self.f(x), self.eps) {
                    roots.add(x, 3);
                } else if self.f(x).is_sign_positive() {
                    let root = self.find_root_at_inf_interval(x, Direction::Left).unwrap();
                    roots.add(root, 1);
                } else {
                    let root = self.find_root_at_inf_interval(x, Direction::Right).unwrap();
                    roots.add(root, 1);
                }

            }
            2 => {
                let x1 = derivative.solutions[0];
                let x2 = derivative.solutions[1];
                let (x1, x2) = if x1 < x2 {(x1, x2)} else {(x2, x1)};

                if is_zero(self.f(x1), self.eps) {
                    roots.add(x1, 2);
                    let root = self.find_root_at_inf_interval(x2, Direction::Right).unwrap();
                    roots.add(root, 1);
                } else if is_zero(self.f(x2), self.eps) {
                    roots.add(x2, 2);
                    let root = self.find_root_at_inf_interval(x1, Direction::Left).unwrap();
                    roots.add(root, 1);
                }

                return roots;

                if self.f(x2).is_sign_positive() {
                    let root = self.find_root_at_inf_interval(x1, Direction::Left).unwrap();
                    roots.add(root, 1);
                }
                if self.f(x1).is_sign_negative() {
                    let root = self.find_root_at_inf_interval(x2, Direction::Right).unwrap();
                    roots.add(root, 1);
                }

                if self.f(x2).is_sign_negative() {
                    let root1 = self.find_root_at_inf_interval(x1, Direction::Left).unwrap();
                    let root2 = self.find_root_at_interval(x1, x2).unwrap();
                    let root3 = self.find_root_at_inf_interval(x2, Direction::Right).unwrap();
                    roots.add(root1, 1);
                    roots.add(root2, 1);
                    roots.add(root3, 1);

                }
            }
            _ => { panic!() }
        }

        return roots;
    }
}

impl fmt::Display for CubicEquation {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}x^3 + {}x^2 + {}x + {} = 0", self.a, self.b, self.c, self.d)?;
        Ok(())
    }
}
