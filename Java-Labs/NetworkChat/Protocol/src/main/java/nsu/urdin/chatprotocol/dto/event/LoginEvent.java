package nsu.urdin.chatprotocol.dto.event;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;
import nsu.urdin.chatprotocol.entity.User;

@Getter
public class LoginEvent extends EventBase {
    private final User user;

    @JsonCreator
    public LoginEvent(@JsonProperty("user") User user) {
        super("userlogin");
        this.user = user;
    }
}
