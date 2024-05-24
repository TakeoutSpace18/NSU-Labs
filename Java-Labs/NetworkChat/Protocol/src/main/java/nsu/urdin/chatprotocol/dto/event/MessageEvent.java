package nsu.urdin.chatprotocol.dto.event;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;
import nsu.urdin.chatprotocol.entity.Message;

@Getter
public class MessageEvent extends EventBase {
    private final Message message;

    @JsonCreator
    public MessageEvent(@JsonProperty("message") Message message) {
        super("message");
        this.message = message;
    }
}
