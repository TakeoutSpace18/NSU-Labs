package nsu.urdin.chatprotocol;

import nsu.urdin.chatprotocol.dto.ErrorResponse;

public class NotAuthenticatedResponse extends ErrorResponse {
    public NotAuthenticatedResponse() {
        super("You are not authenticated");
    }
}
