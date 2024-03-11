package nsu.urdin.tetris.utils;

public record Vec2d(double x, double y) {
    public Vec2d add(Vec2d other) {
        return new Vec2d(x + other.x(), y + other.y());
    }

    public static Vec2d of(double x, double y) {
        return new Vec2d(x, y);
    }
}
