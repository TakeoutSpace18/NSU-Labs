package nsu.urdin.chatprotocol.dto;

public class NotAuthenticatedResponse extends ErrorResponse {
    public NotAuthenticatedResponse() {
        super("You are not authenticated");
    }
}
