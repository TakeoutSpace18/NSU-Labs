package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.ChatHistoryRequest;
import nsu.urdin.chatprotocol.dto.request.ChatHistorySuccessResponse;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatserver.ConnectionSession;

public class GetChatHistoryHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
        if (requestDto instanceof ChatHistoryRequest chatHistoryRequest) {
            if (!session.isLoggedIn()) {
                return new NotAuthenticatedResponse();
            }

            return ChatHistorySuccessResponse.builder()
                    .message(new Message("user123", "hello!"))
                    .message(new Message("user678", "may the force be with you"))
                    .build();

        }

        return null;
    }
}
