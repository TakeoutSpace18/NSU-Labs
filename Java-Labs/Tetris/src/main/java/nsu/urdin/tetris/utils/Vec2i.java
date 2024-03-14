package nsu.urdin.tetris.utils;

public record Vec2i(int x, int y) {
    public Vec2i add(Vec2i other) {
        return new Vec2i(x + other.x(), y + other.y());
    }
    public Vec2i add(int other_x, int other_y) {
        return new Vec2i(x + other_x, y + other_y);
    }
    public Vec2i subtract(Vec2i other) {
        return new Vec2i(x - other.x(), y - other.y());
    }
    public Vec2i subtract(int other_x, int other_y) {
        return new Vec2i(x - other_x, y - other_y);
    }
    public Vec2i scalarMul(int scalar) {
        return new Vec2i(x * scalar, y * scalar);
    }
    public static Vec2i of(int x, int y) {
        return new Vec2i(x, y);
    }
}
