package nsu.urdin.chatprotocol.dto.request;

import com.fasterxml.jackson.annotation.JsonCreator;

public class LogoutRequest extends RequestBase {

    @JsonCreator
    public LogoutRequest() {
        super("logout");
    }
}
