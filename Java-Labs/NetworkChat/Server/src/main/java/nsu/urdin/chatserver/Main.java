package nsu.urdin.chatserver;

public class Main {
    public static void main(String[] args) {
        ChatServer app = new ChatServer();
        Runtime.getRuntime().addShutdownHook(new Thread(app::stop));
        app.run();
    }
}
