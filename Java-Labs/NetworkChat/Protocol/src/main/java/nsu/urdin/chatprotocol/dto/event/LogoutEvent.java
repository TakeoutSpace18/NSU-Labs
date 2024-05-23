package nsu.urdin.chatprotocol.dto.event;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;

@Getter
public class LogoutEvent extends EventBase {
    private final String name;

    @JsonCreator
    public LogoutEvent(@JsonProperty("name") String name) {
        super("userlogout");
        this.name = name;
    }
}
