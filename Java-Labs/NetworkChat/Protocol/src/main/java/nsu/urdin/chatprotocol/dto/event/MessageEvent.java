package nsu.urdin.chatprotocol.dto.event;

import lombok.Getter;
import nsu.urdin.chatprotocol.entity.Message;

@Getter
public class MessageEvent extends EventBase {
    private final Message message;

    public MessageEvent(Message message) {
        super("message");
        this.message = message;
    }
}
