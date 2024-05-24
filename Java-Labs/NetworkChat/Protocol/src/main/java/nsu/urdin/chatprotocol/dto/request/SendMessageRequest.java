package nsu.urdin.chatprotocol.dto.request;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;

@Getter
public class SendMessageRequest extends RequestBase {
    private final String message;

    @JsonCreator
    public SendMessageRequest(@JsonProperty("message") String message) {
        super("message");
        this.message = message;
    }
}
