package nsu.urdin.chatprotocol.dto.request;

import lombok.Getter;

@Getter
public class SendMessageRequest extends RequestBase {
    private final String message;

    public SendMessageRequest(String message) {
        super("message");
        this.message = message;
    }
}
