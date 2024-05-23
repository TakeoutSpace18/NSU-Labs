package nsu.urdin.chatprotocol.dto.response;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;

@Getter
public class ErrorResponse extends ResponseBase {
    private final String message;

    @JsonCreator
    public ErrorResponse(@JsonProperty("message") String message) {
        super("Error");
        this.message = message;
    }
}
