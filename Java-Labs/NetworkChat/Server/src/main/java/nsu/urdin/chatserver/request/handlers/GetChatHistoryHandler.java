package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.ChatHistoryRequest;
import nsu.urdin.chatprotocol.dto.request.ChatHistorySuccessResponse;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.entity.Message;

public class GetChatHistoryHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto) {
        if (requestDto instanceof ChatHistoryRequest chatHistoryRequest) {
            return ChatHistorySuccessResponse.builder()
                    .message(new Message("user123", "hello!"))
                    .message(new Message("user678", "may the force be with you"))
                    .build();

        }

        return null;
    }
}
