package nsu.urdin.chatprotocol.dto;

import lombok.Getter;

@Getter
public class ErrorReply extends ReplyBase {
    private final String message;

    public ErrorReply(String message) {
        super("Error");
        this.message = message;
    }
}
