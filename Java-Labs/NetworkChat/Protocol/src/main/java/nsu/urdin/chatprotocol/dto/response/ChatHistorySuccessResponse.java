package nsu.urdin.chatprotocol.dto.response;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Builder;
import lombok.Getter;
import lombok.Singular;
import nsu.urdin.chatprotocol.entity.Message;

import java.util.List;

@Builder
@Getter
public class ChatHistorySuccessResponse extends SuccessResponse {
    @Singular
    private List<Message> messages;

    @JsonCreator
    public ChatHistorySuccessResponse(@JsonProperty("messages") List<Message> messages) {
        this.messages = messages;
    }
}
