package nsu.urdin.chatprotocol.dto.response;

public class NotAuthenticatedResponse extends ErrorResponse {
    public NotAuthenticatedResponse() {
        super("You are not authenticated");
    }
}
