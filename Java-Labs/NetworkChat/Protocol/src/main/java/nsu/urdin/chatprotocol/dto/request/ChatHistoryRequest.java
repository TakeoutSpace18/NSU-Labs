package nsu.urdin.chatprotocol.dto.request;

import lombok.Getter;

@Getter
public class ChatHistoryRequest extends RequestBase {
    public ChatHistoryRequest() {
        super("chathistory");
    }
}


