package nsu.urdin.tetris.utils;

public record Vec2d(double x, double y) {
    public Vec2d add(Vec2d other) {
        return new Vec2d(x + other.x(), y + other.y());
    }
    public Vec2d add(double other_x, double other_y) {
        return new Vec2d(x + other_x, y + other_y);
    }
    public Vec2d subtract(Vec2d other) {
        return new Vec2d(x - other.x(), y - other.y());
    }
    public Vec2d subtract(double other_x, double other_y) {
        return new Vec2d(x - other_x, y - other_y);
    }
    public Vec2d scalarMul(double scalar) {
        return new Vec2d(x * scalar, y * scalar);
    }

    public boolean equalsEps(Vec2d other, double eps) {
        return (Math.abs(x - other.x()) < eps && Math.abs(y - other.y()) < eps);
    }

    public static Vec2d of(double x, double y) {
        return new Vec2d(x, y);
    }
}
