package nsu.urdin.chatprotocol.dto.request;

import lombok.Builder;
import lombok.Getter;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.entity.Message;

import java.util.List;

@Builder
@Getter
public class ChatHistorySuccessResponse extends SuccessResponse {
    private List<Message> messages;
}
