package nsu.urdin.chatprotocol.dto.request;

import com.fasterxml.jackson.annotation.JsonCreator;
import lombok.Getter;

@Getter
public class ChatHistoryRequest extends RequestBase {
    @JsonCreator
    public ChatHistoryRequest() {
        super("chathistory");
    }
}


