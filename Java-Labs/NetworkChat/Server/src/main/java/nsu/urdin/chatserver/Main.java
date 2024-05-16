package nsu.urdin.chatserver;

public class Main {
    public static void main(String[] args) {
        ChatServer app = ChatServer.getInstance();
        Runtime.getRuntime().addShutdownHook(new Thread(app::stop));
        app.run();
    }
}
