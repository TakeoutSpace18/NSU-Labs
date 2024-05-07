package nsu.urdin.chatprotocol.dto.event;

public class LoginEvent extends EventBase {
    private final String name;

    public LoginEvent(String name) {
        super("userlogin");
        this.name = name;
    }
}
