package nsu.urdin.tetris.utils;

public record Vec2i(int x, int y) {
    public Vec2i add(Vec2i other) {
        return new Vec2i(x + other.x(), y + other.y());
    }

    public static Vec2i of(int x, int y) {
        return new Vec2i(x, y);
    }
}
