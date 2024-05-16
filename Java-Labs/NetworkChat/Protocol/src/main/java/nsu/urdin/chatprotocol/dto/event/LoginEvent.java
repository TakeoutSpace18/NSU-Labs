package nsu.urdin.chatprotocol.dto.event;

import lombok.Getter;
import nsu.urdin.chatprotocol.entity.User;

@Getter
public class LoginEvent extends EventBase {
    private final User user;

    public LoginEvent(User user) {
        super("userlogin");
        this.user = user;
    }
}
