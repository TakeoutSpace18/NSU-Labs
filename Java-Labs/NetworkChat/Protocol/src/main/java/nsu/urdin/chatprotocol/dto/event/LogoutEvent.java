package nsu.urdin.chatprotocol.dto.event;

public class LogoutEvent extends EventBase {
    private final String name;

    public LogoutEvent(String name) {
        super("userlogout");
        this.name = name;
    }
}
