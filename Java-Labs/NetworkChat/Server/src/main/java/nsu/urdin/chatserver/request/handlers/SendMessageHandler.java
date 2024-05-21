package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.dto.event.MessageEvent;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.request.SendMessageRequest;
import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.ConnectionSession;

public class SendMessageHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
        if (requestDto instanceof SendMessageRequest sendMessageRequest) {
            if (!session.isLoggedIn()) {
                return new NotAuthenticatedResponse();
            }

            //TODO: save message to database

            Message message = Message.builder()
                    .userName(session.getUser().getName())
                    .text(sendMessageRequest.getMessage())
                    .build();

            ChatServer.getInstance().broadcastEvent(new MessageEvent(message), session, true);

            return new SuccessResponse();
        }
        return null;
    }
}
