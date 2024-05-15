package nsu.urdin.chatprotocol.dto.event;

import lombok.Getter;

@Getter
public class MessageEvent extends EventBase {
    private final String message;
    private final String from;

    public MessageEvent(String message, String from) {
        super("message");
        this.message = message;
        this.from = from;
    }
}
