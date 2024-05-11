package nsu.urdin.chatprotocol.dto;

import lombok.Getter;

@Getter
public class ErrorResponse extends ResponseBase {
    private final String message;

    public ErrorResponse(String message) {
        super("Error");
        this.message = message;
    }
}
